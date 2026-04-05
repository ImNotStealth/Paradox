#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;
	
		virtual void Init() = 0;
		virtual void WaitIdle() = 0;

		static Unique<GraphicsContext> Create();
	};
}