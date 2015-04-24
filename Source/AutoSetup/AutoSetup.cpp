
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

					ShowError("Could not create wait event!");
					break;
				}

				::ResetEvent(g_hWakeEvent);

				if ((g_hThreadHandle = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, NULL, 0, NULL)) == INVALID_HANDLE_VALUE)
				{
					::CloseHandle(g_hWakeEvent);
					g_hWakeEvent = NULL;
					g_hThreadHandle = NULL;

					ShowError("Could not create thread!");
					break;
				}

				if (!g_pHookManager)
				{
					g_pHookManager.reset(new HookManager());
					g_pHookManager->EnableHook();
				}
			}
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			if (g_hThreadHandle != NULL)
			{
				if (g_pHookManager)
				{
					g_pHookManager->DisableHook();
				}

				::SetEvent(g_hWakeEvent);
				
				if (::WaitForSingleObject(g_hThreadHandle, INFINITE) != WAIT_OBJECT_0)
				{
					ShowError("Could not signal thread!");
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
