#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	enum class GraphicsAPIType
	{
		None = 0,
		Vulkan
	};

	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;
	
		virtual void Init() = 0;
		virtual void WaitIdle() = 0;

		static Shared<GraphicsContext> Create();
		static GraphicsAPIType GetGraphicsAPI() { return s_GraphicsAPI; }
		static void SetGraphicsAPI(GraphicsAPIType api) { s_GraphicsAPI = api; }

	private:
		static GraphicsAPIType s_GraphicsAPI;
	};
}