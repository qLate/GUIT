#pragma once

#include <iostream>
#include <string>

class Debug
{
public:
	inline static int depth = 0;
	inline static bool log = true;
	inline static std::string last = "";

	static void funcEntered(const char* funcName)
	{
		if (!log || depth == -1) return;
		if (depth == 0 && last == funcName)
		{
			depth = -1;
			return;
		}

		for (size_t i = 0; i < depth; i++)
			std::cout << "  ";
		std::cout << funcName << "()" << std::endl;

		if (depth == 0)
			last = funcName;
		depth++;
	}

	static void funcExit(const char* funcName)
	{
		if (!log) return;
		if (depth == -1)
		{
			if (last == funcName)
				depth = 0;
			return;
		}

		depth--;

		for (size_t i = 0; i < depth; i++)
			std::cout << "  ";
		std::cout << "end\n";

		if (depth == 0) std::cout << std::endl;
	}
};
