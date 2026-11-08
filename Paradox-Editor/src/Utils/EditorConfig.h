#pragma once

#include <Paradox/Renderer/GraphicsContext.h>

namespace Paradox
{
	struct EditorConfigProperties
	{
		std::filesystem::path engineSourcePath;
		std::string cmakeGenerator;
		bool openPackagedFolder;
		GraphicsAPIType graphicsApi;
		bool vsync;
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