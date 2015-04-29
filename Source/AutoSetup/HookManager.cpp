
#include <stdio.h>
#include "stdafx.hpp"
#include "HookManager.hpp"
#include "Hook.hpp"
#include <MinHook.h>
#include <string>

HookManager::HookManager() :
	m_bHooked(false)
{
	if (MH_Initialize() != MH_OK)
	{
		ShowError("Unable to initialize MinHook!");
	}

	m_hooks.push_back(std::unique_ptr<Hook>(new Hook(USER32_DLL, "CreateWindowExA", &Hook_CreateWindowExA, (LPVOID *)&fpWin_CreateWindowExA)));
	m_hooks.push_back(std::unique_ptr<Hook>(new Hook(USER32_DLL, "CreateWindowExW", &Hook_CreateWindowExW, (LPVOID *)&fpWin_CreateWindowExW)));
	
	m_hooks.push_back(std::unique_ptr<Hook>(new Hook(USER32_DLL, "GetMessageA", &Hook_GetMessageA, (LPVOID *)&fpWin_GetMessageA)));
	m_hooks.push_back(std::unique_ptr<Hook>(new Hook(USER32_DLL, "GetMessageW", &Hook_GetMessageW, (LPVOID *)&fpWin_GetMessageW)));
	
	m_hooks.push_back(std::unique_ptr<Hook>(new Hook(KERNEL32_DLL, "ExitProcess", &Hook_ExitProcess, (LPVOID *)&fpWin_ExitProcess)));
	m_hooks.push_back(std::unique_ptr<Hook>(new Hook(KERNEL32_DLL, "TerminateProcess", &Hook_TerminateProcess, (LPVOID *)&fpWin_TerminateProcess)));
	
	m_hooks.push_back(std::unique_ptr<Hook>(new Hook(KERNEL32_DLL, "CreateProcessA", &Hook_CreateProcessA, (LPVOID *)&fpWin_CreateProcessA)));
	m_hooks.push_back(std::unique_ptr<Hook>(new Hook(KERNEL32_DLL, "CreateProcessW", &Hook_CreateProcessW, (LPVOID *)&fpWin_CreateProcessW)));
}

void HookManager::EnableHook()
{
	if (m_bHooked)
	{
		return;
	}
	m_bHooked = true;

	auto itr = m_hooks.begin();
	while (itr != m_hooks.end())
	{
		Hook * pHook = (*itr).get();
		MH_STATUS nStatus;

		if ((nStatus = MH_CreateHookApi(pHook->GetModule(), pHook->GetFunction(), pHook->GetOriginalFunctionPtr(), pHook->GetHookFunctionPtr())) != MH_OK)
		{
			char szError[1024] = {0};
			sprintf_s(szError, "CreateHookApi failed!\r\nError code: %d (0x%08x)\r\nModule: %s\r\nFunction: %s", nStatus, nStatus, Narrow(pHook->GetModule()).c_str(), pHook->GetFunction());
			ShowError(szError);
		}
		
		LPVOID pTarget = ::GetProcAddress(::GetModuleHandleW(pHook->GetModule()), pHook->GetFunction());

		if ((nStatus = MH_EnableHook(pTarget)) != MH_OK)
		{
			char szError[1024] = {0};
			sprintf_s(szError, "EnableHook failed!\r\nError code: %d (0x%08x)\r\nModule: %s\r\nFunction: %s", nStatus, nStatus, Narrow(pHook->GetModule()).c_str(), pHook->GetFunction());
			ShowError(szError);
			::ExitProcess(1);
		}

		itr++;
	}

	m_eventHandler.OnStartProcess(::GetModuleHandle(NULL));
}

void HookManager::DisableHook()
{
	if (!m_bHooked)
	{
		return;
	}
	m_bHooked = false;

	MH_Uninitialize();
}

HWND HookManager::CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	HWND hWnd = fpWin_CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	m_eventHandler.OnWindowCreated(dwExStyle, dwStyle, Widen(lpWindowName), hWnd);
	return hWnd;
}

HWND HookManager::CreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	HWND hWnd = fpWin_CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	m_eventHandler.OnWindowCreated(dwExStyle, dwStyle, std::wstring(lpWindowName), hWnd);
	return hWnd;
}

BOOL HookManager::GetMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
	BOOL bReturn = fpWin_GetMessageA(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);

	if (lpMsg->message == WM_QUIT)
	{
	}

	return bReturn;
}

BOOL HookManager::GetMessageW(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
	return GetMessageA(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
}

VOID HookManager::ExitProcess(UINT uExitCode)
{
	m_eventHandler.OnExitProcess(::GetModuleHandle(NULL), uExitCode);
	return fpWin_ExitProcess(uExitCode);
}

BOOL HookManager::TerminateProcess(HANDLE hProcess, UINT uExitCode)
{
	m_eventHandler.OnExitProcess(hProcess, uExitCode);
	return fpWin_TerminateProcess(hProcess, uExitCode);
}

BOOL HookManager::CreateProcessA(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
	m_eventHandler.OnProcessCreating(bInheritHandles, dwCreationFlags);

	BOOL bReturn = fpWin_CreateProcessA(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);

	m_eventHandler.OnProcessCreated(Widen(lpApplicationName), Widen(lpCommandLine), bInheritHandles, dwCreationFlags, Widen(lpCurrentDirectory), lpProcessInformation);

	return bReturn;
}

BOOL HookManager::CreateProcessW(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
	m_eventHandler.OnProcessCreating(bInheritHandles, dwCreationFlags);

	BOOL bReturn = fpWin_CreateProcessW(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	
	m_eventHandler.OnProcessCreated(std::wstring(lpApplicationName), std::wstring(lpCommandLine), bInheritHandles, dwCreationFlags, std::wstring(lpCurrentDirectory), lpProcessInformation);
	
	return bReturn;
}
