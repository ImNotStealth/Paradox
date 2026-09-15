#include "pxpch.h"
#include "UUID.h"

#include <random>
#include <regex>

namespace Paradox {

	static std::mt19937 m_Random(std::random_device{}());
	static std::uniform_int_distribution<int> m_Dist(0, 15);
	static std::uniform_int_distribution<int> m_DistVariant(8, 11); // V4 UUIDs have 

	UUID::UUID()
	{
		std::ostringstream oss;
		oss << std::hex;
		for (int i = 0; i < 12; i++)
			oss << m_Dist(m_Random);
		oss << "4"; // This indicates the version of the UUID
		for (int i = 0; i < 3; i++)
			oss << m_Dist(m_Random);
		oss << m_DistVariant(m_Random);
		for (int i = 0; i < 15; i++)
			oss << m_Dist(m_Random);

		m_ID = oss.str();
	}

	UUID::UUID(const std::string& uuid)
	{
		PX_CORE_ASSERT(IsValid(uuid), "String does not match UUID format.");
		m_ID = uuid;
	}

	bool UUID::IsValid(const std::string& uuid)
	{
		static const std::regex pattern("^[0-9a-fA-F]{12}4[0-9a-fA-F]{3}[89abAB][0-9a-fA-F]{15}$");
		return std::regex_match(uuid, pattern);
	}
}
