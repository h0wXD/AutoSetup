
#include "stdafx.hpp"
#include "HookManager.hpp"

#include <mutex>
#include <thread>
#include <string>
#include <process.h>

HANDLE g_hWakeEvent = NULL;
HANDLE g_hThreadHandle = NULL;

unsigned int __stdcall ThreadProc(void *_pParam) 
{
	HookManager hookManager;

	hookManager.Hook();

	::WaitForSingleObject(g_hWakeEvent, INFINITE);

	return 0;
}

BOOL WINAPI DllMain(HMODULE _hDllHandle, DWORD _dwReason, LPVOID _lpReserved)
{
	switch (_dwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			if (g_hThreadHandle == NULL)
			{
				if ((g_hWakeEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL)) == INVALID_HANDLE_VALUE)
				{
					g_hWakeEvent = NULL;

					MessageBox(NULL, "Could not create wait event!", "Error Occurred!", MB_ICONEXCLAMATION | MB_ICONERROR);
					break;
				}

				::ResetEvent(g_hWakeEvent);

				if ((g_hThreadHandle = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, NULL, 0, NULL)) == INVALID_HANDLE_VALUE)
				{
					::CloseHandle(g_hWakeEvent);
					g_hWakeEvent = NULL;
					g_hThreadHandle = NULL;

					MessageBox(NULL, "Could not create thread!", "Error Occurred!", MB_ICONEXCLAMATION | MB_ICONERROR);
					break;
				}
			}
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			if (g_hThreadHandle != NULL)
			{
				::SetEvent(g_hWakeEvent);
				
				if (::WaitForSingleObject(g_hThreadHandle, INFINITE) != WAIT_OBJECT_0)
				{
					MessageBox(NULL, "Could not signal thread!", "Error Occurred!", MB_ICONEXCLAMATION | MB_ICONERROR);
				}

				::CloseHandle(g_hThreadHandle);
				g_hThreadHandle = NULL;

				if (g_hWakeEvent != NULL)
				{
					::CloseHandle(g_hWakeEvent);
					g_hWakeEvent = NULL;
				}
			}

			break;
		}
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
	}

    return TRUE;
}
