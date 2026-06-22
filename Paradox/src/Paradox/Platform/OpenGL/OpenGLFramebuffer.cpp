#include "pxpch.h"
#include "OpenGLFramebuffer.h"

namespace Paradox
{
	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferProperties& props)
		: m_Props(props)
	{
		OnResize(m_Props.width, m_Props.height);
		PX_CORE_TRACE("Created Framebuffer: {0} ({1}x{2})", m_Props.debugName, m_Props.width, m_Props.height);
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		if (!m_Props.swapchainTarget)
		{
			glDeleteFramebuffers(1, &m_BufferID);
			glDeleteTextures(1, &m_ColorAttachment);
		}
		PX_CORE_TRACE("Destroyed Framebuffer: {0}", m_Props.debugName);
	}

	void OpenGLFramebuffer::OnResize(uint32_t width, uint32_t height)
	{
		if (!m_Props.swapchainTarget)
		{
			if (m_BufferID)
			{
				glDeleteFramebuffers(1, &m_BufferID);
				glDeleteTextures(1, &m_ColorAttachment);

				m_ColorAttachment = 0;
			}

			glCreateFramebuffers(1, &m_BufferID);
			glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);

			glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
			glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#ifndef PX_PLATFORM_PSVITA
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
#endif
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

#ifndef PX_PLATFORM_PSVITA
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(1, buffers);
#endif

			PX_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete.");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		m_Props.width = width;
		m_Props.height = height;
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_Props.swapchainTarget ? 0 : m_BufferID);
		glViewport(0, 0, m_Props.width, m_Props.height);

#ifndef PX_PLATFORM_PSVITA
		if (!m_Props.swapchainTarget)
		{
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(1, buffers);
		}
#endif
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}