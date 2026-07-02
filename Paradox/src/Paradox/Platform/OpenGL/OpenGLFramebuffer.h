#pragma once

#include "Paradox/Renderer/Framebuffer.h"

#include "Paradox/Platform/OpenGL/OpenGL.h"
#include "Paradox/Platform/OpenGL/OpenGLImage.h"

namespace Paradox
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferProperties& props);
		~OpenGLFramebuffer();

		void Resize(uint32_t width, uint32_t height) override;

		void Bind();
		void Unbind();

		FramebufferProperties& GetProperties() override { return m_Props; }
		uint64_t GetImageID() override { return std::static_pointer_cast<OpenGLImage>(m_Attachments[0])->GetHandle(); };

		Shared<Image> GetAttachmentImage(uint32_t index) override { PX_CORE_ASSERT(index < m_Attachments.size()); return m_Attachments[index]; }

	private:
		FramebufferProperties m_Props;
		std::vector<Shared<Image>> m_Attachments;
		std::vector<GLenum> m_GlAttachments;
		uint32_t m_BufferID = 0;
	};
}