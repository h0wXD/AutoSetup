
#include "stdafx.hpp"
#include "CommandLineParser.hpp"
#include <iostream>

void ShowUsage(const char * _pszFileName);
bool LoadExeWithDll(const char *_pszExecutable, const char *_pszDll);

int _tmain(int _iArgumentCount, char *_pszArgumentArray[])
{
	CommandLineParser<char> commandLineParser;
	commandLineParser.AddUnidentifiedKey("-file");
	commandLineParser.Parse(::GetCommandLineA());

	auto pszDllPath = commandLineParser["-d|-dll"];
	auto pszExePath = commandLineParser["-file"];

	if (_iArgumentCount == 1 ||
		commandLineParser.HasAnyFlag("-h|-help") ||
		pszExePath == nullptr)
	{
		ShowUsage(_pszArgumentArray[0]);
		return 1;
	}

	auto pszFile = pszDllPath;

	if (pszFile != nullptr && !FileExists(pszFile) ||
		(pszFile = pszExePath) && !FileExists(pszFile))
	{
		std::cout << "File not found." << std::endl;
		std::cout << "FileName: " << pszFile << std::endl;
		return 1;
	}

	if (!LoadExeWithDll(pszExePath, pszDllPath))
	{
		return 1;
	}

	return 0;
}

void ShowUsage(const char *_pszFileName)
{
	std::string sFileName = _pszFileName;
#ifdef _WIN64
	const char * pszArchitecture = "x64";
#else
	const char * pszArchitecture = "x86";
#endif

	sFileName = sFileName.substr(0, sFileName.find_last_of("\\/"));

	std::cout << std::endl;
	std::cout << "SetupLoader v1.0 " << pszArchitecture << " (" << __DATE__ << " " << __TIME__ << ")" << std::endl;
	std::cout << "Copyright (C) 2015 H0wXD" << std::endl;
	std::cout << "Github - www.github.com/h0wXD" << std::endl;
	std::cout << std::endl;
	std::cout << "usage: " << sFileName << " [-d] file" << std::endl;
	std::cout << "  -d\tOptional. Full path to DLL to load by the executable." << std::endl;
	std::cout << "  file\tFull path to executable to start." << std::endl;
}

bool LoadExeWithDll(const char *_pszExecutable, const char *_pszDll)
{
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInformation;

	memset(&startupInfo, 0, sizeof(startupInfo));
	memset(&processInformation, 0, sizeof(processInformation));

	if (!::CreateProcess(_pszExecutable, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &startupInfo, &processInformation))
	{
		std::cout << "Could not start executable." << std::endl;
		std::cout << "ApplicationName: " << _pszExecutable << std::endl;
		return false;
	}

	HANDLE processHandle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, processInformation.dwProcessId);

	if (processHandle == NULL)
	{
		std::cout << "Could not obtain process handle. OpenProcess failed." << std::endl;
		std::cout << "ProcessId: " << processInformation.dwProcessId << std::endl;
		return false;
	}

	if (_pszDll != nullptr)
	{
		std::size_t nLength;
		BOOL bIsWow64 = FALSE;
		DWORD dwLastError = ERROR_SUCCESS;
		void * pLoadLibrary = nullptr;
		void * pDllEntry = nullptr;

		::IsWow64Process(processHandle, &bIsWow64);

#ifdef WIN64
		if (bIsWow64)
#else
	#ifdef WIN32
		if (!bIsWow64)
	#endif
#endif
		{
			std::cout << "Target executable architecture does not match loader architecture." << std::endl;
#ifdef WIN64
			std::cout << "Architecture: 64-bit" << std::endl;
#else
	#ifdef WIN32
			std::cout << "Architecture: 32-bit" << std::endl;
	#endif
#endif
			::TerminateProcess(processHandle, 1);
			return false;
		}

		nLength = std::strlen(_pszDll);
		pLoadLibrary = (void *)::GetProcAddress(GetModuleHandle(KERNEL32_DLL), LOADLIBRARY);
		
		if (!pLoadLibrary)
		{
			std::cout << "Could not find method " << LOADLIBRARY << "." << std::endl;
			std::cout << "Module: " << KERNEL32_DLL << std::endl;
			::TerminateProcess(processHandle, 1);
			return false;
		}

		::SetLastError(ERROR_SUCCESS);
		pDllEntry = (void *)::VirtualAllocEx(processHandle, NULL, nLength + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

		if (!pDllEntry)
		{
			dwLastError = ::GetLastError();
			std::cout << "Could not allocate memory in suspended process." << std::endl;
			std::printf("ErrorCode: 0x%8x (%d)\n", dwLastError, dwLastError);
			::TerminateProcess(processHandle, 1);
			return false;
		}

		::SetLastError(ERROR_SUCCESS);
		if (::WriteProcessMemory(processHandle, (LPVOID)pDllEntry, _pszDll, nLength, NULL) == FALSE ||
			(dwLastError = ::GetLastError()))
		{
			dwLastError = dwLastError ? dwLastError : ::GetLastError();
			std::cout << "Could not write to process memory." << std::endl;
			std::printf("ErrorCode: 0x%8x (%d)\n", dwLastError, dwLastError);
			::TerminateProcess(processHandle, 1);
			return false;
		}
		
		::SetLastError(ERROR_SUCCESS);
		if (::CreateRemoteThread(processHandle, NULL, NULL, (LPTHREAD_START_ROUTINE)pLoadLibrary, pDllEntry, NULL, NULL) == INVALID_HANDLE_VALUE ||
			(dwLastError = ::GetLastError()))
		{
			dwLastError = dwLastError ? dwLastError : ::GetLastError();
			std::cout << "Could not create remote thread." << std::endl;
			std::printf("ErrorCode: 0x%8x (%d)\n", dwLastError, dwLastError);
			::TerminateProcess(processHandle, 1);
			return false;
		}
	}

	::CloseHandle(processHandle);

	if (!::ResumeThread(processInformation.hThread))
	{
		std::cout << "Could not resume suspended process." << std::endl;
		std::cout << "ThreadId: " << processInformation.hThread << std::endl;
		::TerminateProcess(processHandle, 1);
		return false;
	}

	return true;
}
