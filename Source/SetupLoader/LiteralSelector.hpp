
#pragma once

template <typename T>
struct LiteralSelector;

template<>
struct LiteralSelector<char>
{
    static const char *String(const char *_pszString, const wchar_t *)
    {
        return _pszString;
    }

    static const char Character(const char _cCharacter, const wchar_t)
    {
        return _cCharacter;
    }
};

template<>
struct LiteralSelector<wchar_t>
{
    static const wchar_t *String(const char *, const wchar_t *_pszString)
    {
        return _pszString;
    }

    static const wchar_t Character(const char, const wchar_t _cCharacter)
    {
        return _cCharacter;
    }
};

#define STRING_LITERAL(_STRING, _TYPE) \
	LiteralSelector<_TYPE>::String(_STRING, L ## _STRING)

#define CHARACTER_LITERAL(_CHARACTER, _TYPE) \
	LiteralSelector<_TYPE>::Character(_CHARACTER, L ## _CHARACTER)
