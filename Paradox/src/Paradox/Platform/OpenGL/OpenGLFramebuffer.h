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

		Shared<Image> GetAttachmentImage(uint32_t index) override
		{
#ifdef PX_PLATFORM_PSVITA
			PX_CORE_ASSERT(!ImageUtils::IsDepthFormat(m_ImageFormats[index]), "Getting Depth attachments as Images is unsupported on PS Vita.");
#endif
			PX_CORE_ASSERT(index < m_Attachments.size());
			return m_Attachments[index];
		}

		bool HasDepth() const { return m_HasDepth; }

	private:
		FramebufferProperties m_Props;
		std::vector<Shared<Image>> m_Attachments;
		std::vector<GLenum> m_GlAttachments;
		std::vector<ImageFormat> m_ImageFormats;
		uint32_t m_BufferID = 0;
		bool m_HasDepth = false;
#ifdef PX_PLATFORM_PSVITA
		uint32_t m_DepthAttachmentID = 0;
#endif
	};
}