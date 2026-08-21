#include "pxpch.h"
#include "UUID.h"

#include <random>

namespace Paradox {

	static std::random_device s_RandomDevice;
	static std::mt19937 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<uint32_t> s_UniformDistribution;

	UUID::UUID()
		: m_ID(s_UniformDistribution(s_Engine))
	{}

	UUID::UUID(uint32_t uuid)
		: m_ID(uuid)
	{}
}
