#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Renderer/RendererAPI.h"

namespace Paradox
{
	class Renderer
	{
	public:
		static void Init();

		static const uint32_t GetMaxFramesInFlight() { return 2; }

	private:
		static Unique<RendererAPI> s_RenderAPI;
	};
}