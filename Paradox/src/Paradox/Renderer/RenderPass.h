#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Renderer/Image.h"

namespace Paradox
{
	struct RenderPassProperties
	{
		bool clearColor = true;
		bool swapchainTarget = false;
		std::vector<ImageFormat> attachments = { ImageFormat::RGBA };
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