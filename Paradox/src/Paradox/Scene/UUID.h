#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	class PARADOX_API UUID
	{
	public:
		UUID();
		UUID(const std::string& uuid);
		UUID(const UUID&) = default;

		inline std::string ToString() const { return m_ID; }

		static bool IsValid(const std::string& uuid);

		bool operator==(UUID const& other) const { return m_ID == other.m_ID; }
		bool operator!=(UUID const& other) const { return m_ID != other.m_ID; }

	private:
		std::string m_ID;
	};
}

namespace std
{
	template<>
	struct hash<Paradox::UUID>
	{
		size_t operator()(const Paradox::UUID& uuid) const
		{
			return hash<std::string>()(uuid.ToString());
		}
	};
}