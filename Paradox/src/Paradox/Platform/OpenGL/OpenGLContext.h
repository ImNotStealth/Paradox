#pragma once

#include "Paradox/Renderer/GraphicsContext.h"

namespace Paradox
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		void Init() override;
		void WaitIdle() override {}
	};
}