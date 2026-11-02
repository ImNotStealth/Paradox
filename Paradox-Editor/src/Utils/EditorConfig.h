#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	struct EditorConfigProperties
	{
		std::filesystem::path engineSourcePath;
		bool openPackagedFolder;
	};

	class EditorConfig
	{
	public:
		static void Serialize();
		static void Deserialize();

		static inline EditorConfigProperties& GetProperties() { return m_Properties; }

	private:
		static EditorConfigProperties GetDefaultProperties();

	private:
		static EditorConfigProperties m_Properties;
	};
}