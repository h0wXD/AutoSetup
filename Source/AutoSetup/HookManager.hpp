
#pragma once

#include "Windows.hpp"
#include "Hook.hpp"
#include <unordered_map>

class HookManager
{
	std::vector<std::unique_ptr<Hook>> m_hooks;
	bool m_bHooked;

	CreateWindowExA_t fpWin_CreateWindowExA;
	CreateWindowExW_t fpWin_CreateWindowExW;

	GetMessageA_t fpWin_GetMessageA;
	GetMessageW_t fpWin_GetMessageW;

public:
	HookManager();

	void EnableHook();
	void DisableHook();

public:
	HWND CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
	HWND CreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
	
	BOOL GetMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
	BOOL GetMessageW(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
};
