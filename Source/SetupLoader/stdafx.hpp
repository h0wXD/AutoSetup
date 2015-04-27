
#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <sys/stat.h>

inline bool FileExists(const char * _pszFile)
{
	struct stat st;
	return (0 == stat(_pszFile, &st) 
			&& st.st_mode & _S_IFREG);
}

static const char *KERNEL32_DLL = "kernel32.dll";
static const char *LOADLIBRARY = "LoadLibraryA";
