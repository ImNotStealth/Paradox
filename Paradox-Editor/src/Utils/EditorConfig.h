#pragma once

#include <Paradox/Renderer/GraphicsContext.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Paradox
{
	struct EditorConfigProperties
	{
		std::filesystem::path engineSourcePath;
		std::string cmakeGenerator;
		bool openPackagedFolder;
		GraphicsAPIType graphicsApi;
		bool vsync;

		bool drawGrid;
		glm::vec4 gridColor;
		float gridSize;
		float gridScale;
	};

	class EditorConfig
	{
	public:
		static void Serialize();
		static void Deserialize();

		static inline EditorConfigProperties& GetProperties() { return s_Properties; }

	private:
		static EditorConfigProperties GetDefaultProperties();

	private:
		static EditorConfigProperties s_Properties;
	};
}