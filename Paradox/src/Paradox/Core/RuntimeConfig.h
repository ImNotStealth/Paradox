#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Core/Window.h"
#include "Paradox/Scene/UUID.h"

namespace Paradox
{
	struct RuntimeConfigProperties
	{
		WindowCreateProperties windowProperties;
		UUID startScene = UUID::EMPTY;
	};

	class PARADOX_API RuntimeConfig
	{
	public:
		RuntimeConfig() = default;
		RuntimeConfig(RuntimeConfigProperties& properties)
			: m_Properties(properties) {}

		void Serialize(const std::filesystem::path& filePath);
		bool Deserialize(const std::filesystem::path& filePath);

		RuntimeConfigProperties& GetProperties() { return m_Properties; }

	private:
		void WriteString(std::ostream& out, const std::string& str);
		std::string ReadString(std::istream& in);

	private:
		RuntimeConfigProperties m_Properties;
	};
}