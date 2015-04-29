
#include "stdafx.hpp"
#include "HookManager.hpp"
#include <vector>
#include <string>

void EventHandler::OnStartProcess(HANDLE _hProcess)
{
//	MessageBoxA(NULL, "LET THE FUN BEGIN", "HOOKED", MB_OK | MB_TOPMOST | MB_ICONINFORMATION);
}

void EventHandler::OnExitProcess(HANDLE _hProcess, unsigned int _uiExitCode)
{
	//MessageBoxA(NULL, "LET THE FUN END", "UNHOOKED", MB_OK | MB_TOPMOST | MB_ICONINFORMATION);
}

void EventHandler::OnWindowCreated(DWORD _dwExStyle, DWORD _dwStyle, std::wstring &_sWindowTitle, HWND _hWnd)
{
	if ((_dwStyle & WS_POPUP) != 0)
	{
		wchar_t szTemp[32];
		wchar_t szTest[4096] = {0};
		std::vector<HWND> hWnds;
		HWND hPrevWnd = NULL;
		HWND hNextWnd = NULL;
		
		if ((hNextWnd = ::FindWindowEx(_hWnd, NULL, NULL, NULL)) != NULL)
		{
			do
			{
				memset(szTemp, 0, sizeof(szTemp));
				swprintf(szTemp, sizeof(szTemp) - 1, L"hWnd: 0x%08x", hNextWnd);
				wcscat_s(szTest, szTemp);
				wcscat_s(szTest, L"\r\n");
				hPrevWnd = hNextWnd;
			}
			while (hNextWnd = ::FindWindowEx(_hWnd, hPrevWnd, NULL, NULL));
		}

		if (wcslen(szTest))
		{
			::MessageBoxW(_hWnd, szTemp, _sWindowTitle.c_str(), MB_TOPMOST | MB_OK);
		}
	}
	else
	{
		//wchar_t szTemp[1024] = {0};
		//swprintf(szTemp, sizeof(szTemp) - 1, L"dwExStyle = 0x%08x\r\ndwStyle = 0x%08x", _dwExStyle, _dwStyle);
		//MessageBoxW(_hWnd, szTemp, _sWindowTitle.c_str(), MB_TOPMOST | MB_OK);
	}
}

void EventHandler::OnProcessCreating(BOOL &_bInheritHandles, DWORD &_dwCreationFlags)
{
	_dwCreationFlags |= CREATE_SUSPENDED;
}

void EventHandler::OnProcessCreated(std::wstring &_sApplicationName, std::wstring &_sCommandLine, BOOL _bInheritHandles, DWORD _dwCreationFlags, std::wstring &_sCurrentDirectory, LPPROCESS_INFORMATION _lpProcessInformation)
{
	if (!::ResumeThread(_lpProcessInformation->hThread))
	{
		std::wostringstream sstream;
		sstream << L"Could not resume suspended process." << std::endl;
		sstream << L"ThreadId: " << _lpProcessInformation->hThread << std::endl;
		sstream << L"Application: " << _sApplicationName.c_str() << std::endl;
		sstream << L"CommandLine: " << _sCommandLine.c_str() << std::endl;
		sstream << L"CurrentDirectory: " << _sCurrentDirectory.c_str() << std::endl;
		::MessageBoxW(NULL, sstream.str().c_str(), L"Error Occurred!", MB_TOPMOST | MB_OK);
		::TerminateProcess(_lpProcessInformation->hProcess, 1);
	}
}
