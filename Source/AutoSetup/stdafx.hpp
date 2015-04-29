
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
inline std::string Narrow(const std::wstring &_sSomeString)
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
inline std::string Narrow(const wchar_t *_pszSomeString)
{
	return _pszSomeString == nullptr ? std::string() : Narrow(std::wstring(_pszSomeString));
}

inline std::wstring Widen(const std::string &_sSomeString)
{
    std::wostringstream wstm;
    const std::ctype<wchar_t> & ctfacet = std::use_facet<std::ctype<wchar_t>>(wstm.getloc());
	
    for (size_t i = 0; i < _sSomeString.size(); i++)
	{
		wstm << ctfacet.widen(_sSomeString[i]);
	}

	wstm << L'\0';

    return wstm.str();
}
inline std::wstring Widen(const char *_pszSomeString)
{
	return _pszSomeString == nullptr ? std::wstring() : Widen(std::string(_pszSomeString));
}
