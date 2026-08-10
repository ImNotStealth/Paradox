#include "pxpch.h"
#include "OpenGLFramebuffer.h"

namespace Paradox
{
	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferProperties& props)
		: m_Props(props)
	{
		PX_PROFILE_FUNCTION();
		uint8_t colorAttachmentCount = 0;
		for (const FramebufferAttachment& attachment : m_Props.attachments)
		{
			m_ImageFormats.push_back(attachment.format);

			if (!ImageUtils::IsDepthFormat(attachment.format))
			{
				m_GlAttachments.push_back(GL_COLOR_ATTACHMENT0 + colorAttachmentCount);
				colorAttachmentCount++;
			}
			else
				m_HasDepth = true;

			if (attachment.existingImage)
			{
#ifdef PX_PLATFORM_PSVITA
				PX_CORE_ASSERT(!ImageUtils::IsDepthFormat(attachment.format), "Attaching existing Depth Images is not supported on PS Vita.");
#endif
				m_Attachments.emplace_back(attachment.existingImage);
				continue;
			}

#ifdef PX_PLATFORM_PSVITA
			if (ImageUtils::IsDepthFormat(attachment.format))
			{
				// Depth attachments apparently aren't possible on Vita so pushing back nullptr to keep indexes aligned.
				m_Attachments.push_back(nullptr);
				continue;
			}
#endif

			ImageProperties imageProps = {};
			imageProps.width = props.width;
			imageProps.height = props.height;
			imageProps.usage = ImageUsage::Attachment;
			imageProps.format = attachment.format;
			imageProps.debugName = m_Props.debugName;
			m_Attachments.emplace_back(Image::Create(imageProps));
		}

		Resize(m_Props.width, m_Props.height);
		PX_CORE_TRACE("Created Framebuffer: {0} ({1}x{2})", m_Props.debugName, m_Props.width, m_Props.height);
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		PX_PROFILE_FUNCTION();
		if (!m_Props.swapchainTarget)
		{
#ifdef PX_PLATFORM_PSVITA
			if (m_DepthAttachmentID)
				glDeleteRenderbuffers(1, &m_DepthAttachmentID);
#endif
			glDeleteFramebuffers(1, &m_BufferID);
		}
		PX_CORE_TRACE("Destroyed Framebuffer: {0}", m_Props.debugName);
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		PX_PROFILE_FUNCTION();
		if (!m_Props.swapchainTarget)
		{
			if (m_BufferID)
			{
#ifdef PX_PLATFORM_PSVITA
				if (m_DepthAttachmentID)
					glDeleteRenderbuffers(1, &m_DepthAttachmentID);
#endif
				glDeleteFramebuffers(1, &m_BufferID);
			}

			glCreateFramebuffers(1, &m_BufferID);
			glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);

			for (Shared<Image> image : m_Attachments)
			{
#ifdef PX_PLATFORM_PSVITA
				if (!image)
				{
					glGenRenderbuffers(1, &m_DepthAttachmentID);
					glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachmentID);
					glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthAttachmentID);
					continue;
				}
#endif

				bool isExistingImage = false;
				for (const FramebufferAttachment& attachment : m_Props.attachments)
				{
					if (attachment.existingImage == image)
					{
						isExistingImage = true;
						break;
					}
				}

				if (!isExistingImage)
					image->Resize(width, height);

				Shared<OpenGLImage> glImage = std::static_pointer_cast<OpenGLImage>(image);
				glFramebufferTexture2D(GL_FRAMEBUFFER, ImageUtils::IsDepthFormat(image->GetFormat()) ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glImage->GetHandle(), 0);
			}

#ifndef PX_PLATFORM_PSVITA
			if (!m_GlAttachments.empty())
				glDrawBuffers(m_GlAttachments.size(), m_GlAttachments.data());
			else
				glDrawBuffer(GL_NONE);
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
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}