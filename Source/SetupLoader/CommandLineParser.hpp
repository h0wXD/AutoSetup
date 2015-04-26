
#pragma once

#include "LiteralSelector.hpp"
#include <string>
#include <sstream>
#include <iterator>
#include <unordered_map>
#include <set>

template<typename T>
class CommandLineParser
{
public:
	typedef std::basic_string<T, std::char_traits<T>, std::allocator<T>> String_t;
	typedef std::basic_stringstream<T, std::char_traits<T>, std::allocator<T>> StringStream_t;
	typedef std::unordered_map<String_t, String_t> Map_t;
	typedef std::unordered_map<String_t, bool> BoolMap_t;

private:
	static const size_t N_ALLOC_SIZE = 1024;
	T C_DEFAULT_DELIMITER = CHARACTER_LITERAL('|', T);
	String_t S_SPACE = STRING_LITERAL(" ", T);
	String_t S_TOKENS = STRING_LITERAL(" \"", T);
	String_t S_GROUPS = STRING_LITERAL("\"", T);
	String_t S_EMPTY = STRING_LITERAL("", T);

private:
	T *m_pBuffer;
	Map_t m_map;
	BoolMap_t m_unidentifiedMap;

public:
	CommandLineParser() :
		m_pBuffer(nullptr)
	{  }

	~CommandLineParser()
	{
		Reset();
	}

	inline void AddUnidentifiedKey(const String_t &_pszKey);
	inline void Parse(const String_t &_pszCommandLine);
	inline void Parse(int _iArgumentCount, T *_pszArgumentArray[]);
	inline bool HasFlag(const String_t &_sFlag);
	inline bool HasAnyFlag(const String_t &_sFlags, const T _cDelimiter = CHARACTER_LITERAL('|', T));
	inline const T *operator[](const String_t &_sKey);
	inline const T *GetValue(const String_t &_sKey);
	inline const T *GetAnyValue(const String_t &_sKeys, const T _cDelimiter = CHARACTER_LITERAL('|', T));

private:
	void *Allocate(size_t _nSize);
	void Reset();
	void Split(const String_t &_sCommandLine);
};

template<typename T>
void CommandLineParser<T>::AddUnidentifiedKey(const String_t &_pszKey)
{
	auto itr = m_unidentifiedMap.find(_pszKey);
	if (itr == m_unidentifiedMap.end())
	{
		m_unidentifiedMap[_pszKey] = false;
	}
}

template<typename T>
inline void CommandLineParser<T>::Parse(const String_t &_sCommandLine)
{
	Split(_sCommandLine);
}

template<typename T>
inline void CommandLineParser<T>::Parse(int _iArgumentCount, T *_pszArgumentArray[])
{
	auto ppszEnd = _pszArgumentArray + _iArgumentCount;
	String_t sCommandLine;

	auto ppIterator = _pszArgumentArray;
	while (ppIterator != ppszEnd)
	{
		if (sCommandLine.length() != 0)
		{
			sCommandLine += S_SPACE;
		}

		sCommandLine += *ppIterator;
		ppIterator++;
	}

	Split(sCommandLine);
}

template<typename T>
bool CommandLineParser<T>::HasFlag(const String_t &_sFlag)
{
	return m_map.find(_sFlag) != m_map.end();
}

template<typename T>
bool CommandLineParser<T>::HasAnyFlag(const String_t &_sFlags, const T _cDelimiter /*= '|'*/)
{
	StringStream_t sstream(_sFlags);
	String_t sFlag;

	while (std::getline(sstream, sFlag, _cDelimiter))
	{
		if (m_map.find(sFlag) != m_map.end())
		{
			return true;
		}
	}

	return false;
}

template<typename T>
const T *CommandLineParser<T>::operator[](const String_t &_sKey)
{
	if (String_t(_sKey).find_first_of(C_DEFAULT_DELIMITER) != String_t::npos)
	{
		return GetAnyValue(_sKey);
	}
	return GetValue(_sKey);
}

template<typename T>
const T *CommandLineParser<T>::GetValue(const String_t &_sKey)
{
	auto itr = m_map.find(_sKey);
	return itr == m_map.end() ? nullptr : itr->second.c_str();
}

template<typename T>
const T *CommandLineParser<T>::GetAnyValue(const String_t &_sKeys, const T _cDelimiter /*= '|'*/)
{
	StringStream_t sstream(_sKeys);
	String_t sKey;

	while (std::getline(sstream, sKey, _cDelimiter))
	{
		auto itr = m_map.find(sKey);
		if (itr != m_map.end())
		{
			return itr->second.c_str();
		}
	}

	return nullptr;
}

template<typename T>
void *CommandLineParser<T>::Allocate(size_t _nSize)
{
	auto nAllocSize = (N_ALLOC_SIZE < _nSize) ? int(_nSize / float(N_ALLOC_SIZE) + 1) * N_ALLOC_SIZE : N_ALLOC_SIZE;
	auto pBuffer = new char[nAllocSize];
	return memset(pBuffer, 0, nAllocSize);
}

template<typename T>
void CommandLineParser<T>::Reset()
{
	if (m_pBuffer)
	{
		delete[] m_pBuffer;
		m_pBuffer = nullptr;
	}
	m_map.clear();

	for (auto &keyValue : m_unidentifiedMap)
	{
		keyValue.second = false;
	}
}

template<typename T>
void CommandLineParser<T>::Split(const String_t &_sCommandLine)
{
	Reset();

	if (_sCommandLine.length() == 0)
	{
		return;
	}
	
	size_t nSize = _sCommandLine.length() * sizeof(T);

	m_pBuffer = (T *)Allocate(nSize + (2 * sizeof(T)));
	memcpy(m_pBuffer, _sCommandLine.c_str(), nSize);

	memcpy(&m_pBuffer[nSize], S_SPACE.c_str(), sizeof(T));

	String_t sString = m_pBuffer;

	size_t nWordStart = sString.find_first_not_of(S_TOKENS);
	T * pszPreviousKey = nullptr;

	std::set<String_t> previousKeys;

	while (String_t::npos != nWordStart)
	{
		String_t sGroupEnd;
		bool bGroupMember = false;

		if (nWordStart > 0)
		{
			sGroupEnd = sString[nWordStart - 1];
			bGroupMember = String_t::npos != S_GROUPS.find(sGroupEnd);
		}

		size_t nWordEnd = sString.find_first_of(bGroupMember ? sGroupEnd : S_TOKENS, nWordStart);
		
		memset(&m_pBuffer[nWordEnd], 0, sizeof(T));

		if ('-' == m_pBuffer[nWordStart])
		{
			pszPreviousKey = &m_pBuffer[nWordStart];
			previousKeys.insert(pszPreviousKey);
		}
		else
		{
			if (pszPreviousKey)
			{
				if (m_map.find(pszPreviousKey) == m_map.end())
				{
					m_map[pszPreviousKey] = &m_pBuffer[nWordStart];
				}
				else
				{
					for (auto &keyValue : m_unidentifiedMap)
					{
						if (!keyValue.second)
						{
							m_map[keyValue.first] = &m_pBuffer[nWordStart];
							keyValue.second = true;
							break;
						}
					}
				}
			}
		}
		
		nWordStart = sString.find_first_not_of(S_TOKENS, nWordEnd);
	}

	for (auto &sPreviousKey : previousKeys)
	{
		auto itr = m_map.find(sPreviousKey);
		if (itr == m_map.end())
		{
			m_map[sPreviousKey] = S_EMPTY;
		}
	}
}
