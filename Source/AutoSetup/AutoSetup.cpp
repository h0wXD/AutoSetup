
#include "stdafx.hpp"
#include "HookManager.hpp"

#include <memory>
#include <thread>

std::thread *g_pHookThread = nullptr;
bool g_bIsRunning = false;

BOOL WINAPI DllMain(HMODULE hDllHandle, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			if (g_pHookThread == nullptr)
			{
				g_bIsRunning = true;
				g_pHookThread = new std::thread();
			}
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			if (g_pHookThread != nullptr)
			{
				if (g_pHookThread->joinable())
				{
					g_pHookThread->join();
				}
				delete g_pHookThread;
			}
			break;
		}
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
	}

    return TRUE;
}
