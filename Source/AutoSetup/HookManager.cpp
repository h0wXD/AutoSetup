
#include "stdafx.hpp"
#include "HookManager.hpp"
#include <detours.h>
#pragma comment(lib, "detours.lib")
#include <string>

HookManager::HookManager()
{
	HMODULE hUserModule = GetModuleHandle("user32.dll");
	HMODULE hKernelModule = GetModuleHandle("kernel32.dll");

	#ifdef UNICODE
		pWin_CreateWindowEx = (t_CreateWindowEx)GetProcAddress(hUserModule, "CreateWindowExW");
	#else
		pWin_CreateWindowEx = (t_CreateWindowEx)GetProcAddress(hUserModule, "CreateWindowExA");
	#endif
}

void HookManager::Hook()
{
	MessageBoxA(NULL, "LET THE FUN BEGIN", "HOOKED", MB_OK | MB_TOPMOST | MB_ICONINFORMATION);

	::DetourTransactionBegin();
	::DetourUpdateThread(GetCurrentThread());

	::DetourAttach(&(PVOID&)pWin_CreateWindowEx, Hook_CreateWindowEx);

	::DetourTransactionCommit();
}

HWND HookManager::CreateWindowEx(DWORD dwExStyle, asLPCSTR lpClassName, asLPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	//MessageBoxA(NULL, (std::string("CREATING WINDOW: ") + lpWindowName).c_str(), "HOOKED", MB_OK | MB_TOPMOST | MB_ICONINFORMATION);

	return pWin_CreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}
