#include "pxpch.h"
#include "CommandLineParser.h"

namespace Paradox
{
	CommandLineParser::CommandLineParser(int argc, char** argv)
	{
		for (int i = 1; i < argc; ++i)
		{
			std::string arg = argv[i];
			if (arg.substr(0, 2) != "--")
				continue;

			std::string flag = arg.substr(2);

			if (i + 1 >= argc)
			{
				m_Args[flag] = std::string();
				break;
			}

			std::string next = argv[i + 1];
			if (next.substr(0, 2) != "--")
				m_Args[flag] = next;
			else
				m_Args[flag] = std::string();
		}

		for (const auto& [flag, value] : m_Args)
			PX_CORE_TRACE("Received command argument: --{} = {}", flag, value.empty() ? "N/A" : value);
	}
}