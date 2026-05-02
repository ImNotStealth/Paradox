#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Renderer/RenderPass.h"

#include <glm/glm.hpp>

namespace Paradox
{
	struct FramebufferProperties
	{
		uint32_t width;
		uint32_t height;
		glm::vec4 clearColor;

		std::string debugName;
	};

	class Framebuffer
	{
	public:
		~Framebuffer() = default;

		virtual void OnResize(uint32_t width, uint32_t height) = 0;
		virtual FramebufferProperties& GetProperties() = 0;

		static Shared<Framebuffer> Create(const FramebufferProperties& props);
	};
}