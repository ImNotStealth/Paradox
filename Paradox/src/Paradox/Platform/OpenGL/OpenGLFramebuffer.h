#pragma once

#include "Paradox/Renderer/Framebuffer.h"

#include "Paradox/Platform/OpenGL/OpenGL.h"

namespace Paradox
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferProperties& props);
		~OpenGLFramebuffer();

		void OnResize(uint32_t width, uint32_t height) override;

		void Bind();
		void Unbind();

		FramebufferProperties& GetProperties() override { return m_Props; }
		uint64_t GetImageID() override { return m_ColorAttachment; };

	private:
		FramebufferProperties m_Props;
		uint32_t m_BufferID = 0;
		uint32_t m_ColorAttachment = 0;
	};
}