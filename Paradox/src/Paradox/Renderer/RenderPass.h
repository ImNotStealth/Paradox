#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	struct RenderPassProperties
	{
		std::string debugName;
	};

	// PARADOX_API is temporary
	class PARADOX_API RenderPass
	{
	public:
		virtual ~RenderPass() = default;

		virtual RenderPassProperties& GetProperties() = 0;

		static Shared<RenderPass> Create(const RenderPassProperties& props);
	};
}