#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	struct RenderPassProperties
	{
		bool clearColor = true;
		std::string debugName;
	};

	class PARADOX_API RenderPass
	{
	public:
		virtual ~RenderPass() = default;

		virtual RenderPassProperties& GetProperties() = 0;

		static Shared<RenderPass> Create(const RenderPassProperties& props);
	};
}