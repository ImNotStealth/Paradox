#pragma once

#include "Paradox/Renderer/RendererAPI.h"

namespace Paradox
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		void Init() override;
	};
}