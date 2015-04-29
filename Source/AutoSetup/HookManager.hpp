
#pragma once

#include "Windows.hpp"
#include "Hook.hpp"
#include "EventHandler.hpp"
#include <unordered_map>

class HookManager
{
	std::vector<std::unique_ptr<Hook>> m_hooks;
	bool m_bHooked;
	EventHandler m_eventHandler;

	CreateWindowExA_t fpWin_CreateWindowExA;
	CreateWindowExW_t fpWin_CreateWindowExW;

	GetMessageA_t fpWin_GetMessageA;
	GetMessageW_t fpWin_GetMessageW;
	
	ExitProcess_t fpWin_ExitProcess;
	TerminateProcess_t fpWin_TerminateProcess;

	CreateProcessA_t fpWin_CreateProcessA;
	CreateProcessW_t fpWin_CreateProcessW;

public:
	HookManager();

	void EnableHook();
	void DisableHook();

public:
	HWND CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
	HWND CreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
	
	BOOL GetMessageA(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
	BOOL GetMessageW(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
	
	VOID ExitProcess(UINT uExitCode);
	BOOL TerminateProcess(HANDLE hProcess, UINT uExitCode);

	BOOL CreateProcessA(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);
	BOOL CreateProcessW(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);
};
