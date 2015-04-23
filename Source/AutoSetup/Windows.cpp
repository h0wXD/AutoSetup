
#include "stdafx.hpp"
#include "Windows.hpp"
#include "HookManager.hpp"

std::unique_ptr<HookManager> g_pHookManager;

HWND WINAPI Hook_CreateWindowEx(DWORD dwExStyle, asLPCSTR lpClassName, asLPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	return g_pHookManager->CreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}
