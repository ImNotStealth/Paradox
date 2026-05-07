#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	class RendererAPI
	{
	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;

		static Unique<RendererAPI> Create();
	};
}