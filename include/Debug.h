#pragma once

#include <iostream>
#include <sstream>
#include <string>

enum class LogType
{
	Default = 1,
	Input = 2,
	FunctionCalls = 4,
};

inline LogType operator|(LogType a, LogType b)
{
	return (LogType)((int)a | (int)b);
}
inline bool hasFlag(LogType flags, LogType flag)
{
	return ((int)flags & (int)flag) != 0;
}

class Debug
{
public:
	inline static bool loggingEnabled = true;
	inline static LogType enabledLogTypes = LogType::Default | LogType::FunctionCalls;

	inline static int depth = 0;
	inline static bool preventSpam = true;
	inline static std::string last = "";

	template <typename... P>
	static void log(LogType logType, const P&... params)
	{
		if (!hasFlag(enabledLogTypes, logType)) return;
		std::stringstream stream;
		(stream << ... << params);
		std::cout << stream.str() << std::endl;
	}

	template <typename... P>
	static void funcEntered(const char* funcName, const P&... params)
	{
		if (!loggingEnabled || !hasFlag(enabledLogTypes, LogType::FunctionCalls) || depth == -1) return;
		if (preventSpam && depth == 0 && last == funcName)
		{
			depth = -1;
			return;
		}

		for (size_t i = 0; i < depth; i++)
			std::cout << "  ";

		std::stringstream stream;
		(stream << ... << params);
		std::cout << funcName << "(" << stream.str() << ")" << std::endl;

		if (depth == 0)
			last = funcName;
		depth++;
	}

	static void funcExit(const char* funcName)
	{
		if (!loggingEnabled || !hasFlag(enabledLogTypes, LogType::FunctionCalls)) return;
		if (preventSpam && depth == -1)
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
