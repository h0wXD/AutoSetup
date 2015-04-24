
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
}

void HookManager::EnableHook()
{
	if (m_bHooked)
	{
		return;
	}
	m_bHooked = true;

	MessageBoxA(NULL, "LET THE FUN BEGIN", "HOOKED", MB_OK | MB_TOPMOST | MB_ICONINFORMATION | MB_TOPMOST);

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
		}

		itr++;
	}
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
	return fpWin_CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

HWND HookManager::CreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	return fpWin_CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

BOOL HookManager::GetMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
	BOOL bReturn = fpWin_GetMessageA(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);

	if (lpMsg->message == WM_QUIT)
	{
		MessageBoxA(NULL, "LET THE FUN END", "UNHOOKED", MB_OK | MB_TOPMOST | MB_ICONINFORMATION | MB_TOPMOST);
	}

	return bReturn;
}

BOOL HookManager::GetMessageW(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
	return GetMessageA(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
}
