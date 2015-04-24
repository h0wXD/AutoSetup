
#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#include <Windows.h>
#include <memory>
#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>

inline void ShowError(const char * _pszMessage)
{
	MessageBox(NULL, _pszMessage, "Error Occurred!", MB_ICONEXCLAMATION | MB_ICONERROR | MB_OK | MB_TOPMOST);
}

#pragma warning(disable:4244)
#pragma warning(disable:4018)
inline std::string Narrow(const std::wstring & _sSomeString)
{
    std::ostringstream sstream;
    const std::ctype<char> & ctfacet = std::use_facet<std::ctype<char>>(sstream.getloc());
	
    for (size_t i = 0; i < _sSomeString.size(); i++)
	{
		sstream << ctfacet.narrow(_sSomeString[i], 0);
	}

	sstream << '\0';
	
    return sstream.str();
}
