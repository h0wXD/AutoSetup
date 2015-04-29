
#pragma once

#include "stdafx.hpp"

class EventHandler
{
public:
	void OnStartProcess(HANDLE _hProcess);
	void OnExitProcess(HANDLE _hProcess, unsigned int _uiExitCode);

	void OnWindowCreated(DWORD _dwExStyle, DWORD _dwStyle, std::wstring &_sWindowTitle, HWND _hWnd);

	void OnProcessCreating(BOOL &_bInheritHandles, DWORD &_dwCreationFlags);
	void OnProcessCreated(std::wstring &_sApplicationName, std::wstring &_sCommandLine, BOOL _bInheritHandles, DWORD _dwCreationFlags, std::wstring &_sCurrentDirectory, LPPROCESS_INFORMATION _lpProcessInformation);
};
