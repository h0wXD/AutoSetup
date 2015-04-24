
#pragma once

#include "stdafx.hpp"

class Hook
{
	const LPCWSTR m_pszModule;
	const LPCSTR m_pszFunction;
	const LPVOID m_fpOriginalFunctionPtr;
	LPVOID *m_pfpHookFunctionPtr;

public:
	Hook::Hook(const LPCWSTR _pszModule, const LPCSTR _pszFunction, LPVOID _fpOriginalFunctionPtr, LPVOID *_pfpHookFunctionPtr) :
		m_pszModule(_pszModule),
		m_pszFunction(_pszFunction),
		m_fpOriginalFunctionPtr(_fpOriginalFunctionPtr),
		m_pfpHookFunctionPtr(_pfpHookFunctionPtr)
	{  }

	inline LPCWSTR GetModule()
	{
		return m_pszModule;
	}
	inline LPCSTR GetFunction()
	{
		return m_pszFunction;
	}
	inline LPVOID GetOriginalFunctionPtr()
	{
		return m_fpOriginalFunctionPtr;
	}
	inline LPVOID *GetHookFunctionPtr()
	{
		return m_pfpHookFunctionPtr;
	}
};
