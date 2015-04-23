
#pragma once

#include "Windows.hpp"

class HookManager
{
	t_CreateWindowEx pWin_CreateWindowEx;

public:
	HookManager();
	void Hook();

public:
	HWND CreateWindowEx(DWORD dwExStyle, asLPCSTR lpClassName, asLPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
};
