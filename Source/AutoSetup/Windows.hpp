
#pragma once

#include "stdafx.hpp"

class HookManager;
extern std::unique_ptr<HookManager> g_pHookManager;

#ifdef UNICODE
typedef LPCWSTR asLPCSTR;
#else
typedef LPCSTR asLPCSTR;
#endif

typedef HWND 		(__stdcall *t_CreateWindowEx)			(DWORD, asLPCSTR, asLPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);

HWND WINAPI 		Hook_CreateWindowEx						(DWORD dwExStyle, asLPCSTR lpClassName, asLPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
