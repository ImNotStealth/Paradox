#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	enum class GraphicsAPIType
	{
		None = 0,
		Vulkan,
		OpenGL
	};

	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;
	
		virtual void Init() = 0;
		virtual void WaitIdle() = 0;

		static Shared<GraphicsContext> Create();

		static GraphicsAPIType GetGraphicsAPI() { return s_GraphicsAPI; }
		static void SetGraphicsAPI(GraphicsAPIType api)
		{
			PX_CORE_INFO("GraphicsAPI: {0}", GraphicsAPIToString(api));
			s_GraphicsAPI = api;
		}

		static std::string GraphicsAPIToString(GraphicsAPIType api);

	private:
		static GraphicsAPIType s_GraphicsAPI;
	};
}