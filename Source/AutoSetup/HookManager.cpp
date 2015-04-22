
#include "stdafx.hpp"
#include "HookManager.hpp"

void HookManager::Hook()
{
	MessageBoxA(NULL, "HOOK", "", MB_OK);
}
