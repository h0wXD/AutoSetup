
#include "stdafx.hpp"
#include "CommandLineParser.hpp"
#include <iostream>

void ShowUsage(char *);

int _tmain(int _iArgumentCount, char *_pszArgumentArray[])
{
	CommandLineParser<char> commandLineParser;
	commandLineParser.AddUnidentifiedKey("-file");
	commandLineParser.Parse(::GetCommandLineA());

	auto pszDllPath = commandLineParser["-d|-dll"];
	auto pszExePath = commandLineParser["-file"];

	if (_iArgumentCount == 1 ||
		commandLineParser.HasAnyFlag("-h|-help") ||
		pszExePath == nullptr)
	{
		ShowUsage(_pszArgumentArray[0]);
		return 1;
	}

	return 0;
}

void ShowUsage(char *_pszFileName)
{
	std::string sFileName = _pszFileName;
#ifdef _WIN64
	const char * pszArchitecture = "x64";
#else
	const char * pszArchitecture = "x86";
#endif

	sFileName = sFileName.substr(0, sFileName.find_last_of("\\/"));

	std::cout << std::endl;
	std::cout << "SetupLoader v1.0 " << pszArchitecture << " (" << __DATE__ << " " << __TIME__ << ")" << std::endl;
	std::cout << "Copyright (C) 2015 H0wXD" << std::endl;
	std::cout << "Github - www.github.com/h0wXD" << std::endl;
	std::cout << std::endl;
	std::cout << "usage: " << sFileName << " [-d] file" << std::endl;
	std::cout << "  -d\tOptional. Full path to DLL to load by the executable." << std::endl;
	std::cout << "  file\tFull path to executable to start." << std::endl;
}
