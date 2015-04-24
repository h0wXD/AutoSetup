
#pragma once

#include "stdafx.hpp"

class HookManager;
extern std::unique_ptr<HookManager> g_pHookManager;

#define USER32_DLL		L"user32.dll"
#define KERNEL32_DLL	L"kernel32.dll"

typedef HWND 		(WINAPI *CreateWindowExA_t)			(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
typedef HWND 		(WINAPI *CreateWindowExW_t)			(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
HWND WINAPI 		Hook_CreateWindowExA				(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
HWND WINAPI 		Hook_CreateWindowExW				(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

typedef BOOL		(WINAPI	*GetMessageA_t)				(LPMSG, HWND, UINT, UINT);
typedef BOOL		(WINAPI	*GetMessageW_t)				(LPMSG, HWND, UINT, UINT);
BOOL WINAPI			Hook_GetMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
BOOL WINAPI			Hook_GetMessageW(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
