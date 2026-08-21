#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	class PARADOX_API UUID
	{
	public:
		UUID();
		UUID(uint32_t uuid);
		UUID(const UUID&) = default;

		operator uint32_t() const { return m_ID; }
	private:
		uint32_t m_ID;
	};
}

namespace std {

	template<>
	struct hash<Paradox::UUID>
	{
		std::size_t operator()(const Paradox::UUID& uuid) const
		{
			return hash<uint32_t>()((uint32_t)uuid);
		}
	};
}