#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	class PARADOX_API CommandLineParser
	{
	public:
		CommandLineParser(int argc, char** argv);

		bool HasFlag(const std::string& flag) const { return m_Args.find(flag) != m_Args.end(); }

		template<typename T>
		T GetRequired(const std::string& flag) const
		{
			PX_CORE_ASSERT(HasFlag(flag), "Required argument not found.");

			std::string value = m_Args.at(flag);
			PX_CORE_ASSERT(!value.empty(), "Required argument has no value.");

			if constexpr (std::is_base_of_v<std::string, T>)
				return value;
			
			std::stringstream ss = std::stringstream(value);
			T t;
			ss >> t;
			PX_CORE_ASSERT(!ss.fail(), "Failed to convert argument to required type.");
			return t;
		}

		template<typename T>
		T GetOrDefault(const std::string& flag, T defaultValue) const
		{
			if (!HasFlag(flag))
				return defaultValue;

			std::string value = m_Args.at(flag);
			if (value.empty())
			{
				PX_CORE_WARN("Flag '{0}' provided no value, using default value.", flag);
				return defaultValue;
			}

			if constexpr (std::is_base_of_v<std::string, T>)
				return value;

			std::stringstream ss = std::stringstream(value);
			T t;
			ss >> t;
			PX_CORE_ASSERT(!ss.fail(), "Failed to convert argument to required type.");
			return t;
		}

		const std::unordered_map<std::string, std::string>& GetArgs() const { return m_Args; }

	private:
		std::unordered_map<std::string, std::string> m_Args;
	};
}