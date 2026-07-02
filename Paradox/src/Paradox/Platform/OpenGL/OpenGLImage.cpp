#include "pxpch.h"
#include "OpenGLImage.h"

#include "Paradox/Platform/OpenGL/OpenGL.h"

namespace Paradox
{
	OpenGLImage::OpenGLImage(const ImageProperties& props)
		: m_Properties(props)
	{
		Resize(props.width, props.height);
	}

	OpenGLImage::~OpenGLImage()
	{
		glDeleteTextures(1, &m_Handle);
	}

	void OpenGLImage::Resize(uint32_t width, uint32_t height)
	{
		if (m_Handle)
			glDeleteTextures(1, &m_Handle);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);

#ifdef PX_PLATFORM_PSVITA
		glBindTexture(GL_TEXTURE_2D, m_Handle);
		glTexImage2D(GL_TEXTURE_2D, 0, OpenGLUtils::GetOpenGLInternalFormat(m_Properties.format), width, height, 0, OpenGLUtils::GetOpenGLFormat(m_Properties.format), GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
#else
		glTextureStorage2D(m_Handle, 1, OpenGLUtils::GetOpenGLInternalFormat(m_Properties.format), width, height);
#endif

		m_Properties.width = width;
		m_Properties.height = height;
	}

	void OpenGLImage::SetData(void* data, uint32_t size)
	{
		if (m_Buffer.empty() || m_Buffer.size() < size)
			m_Buffer.resize(size);

		memcpy(m_Buffer.data(), data, size);
#ifdef PX_PLATFORM_PSVITA
		glBindTexture(GL_TEXTURE_2D, m_Handle);
		glTexImage2D(GL_TEXTURE_2D, 0, OpenGLUtils::GetOpenGLInternalFormat(m_Properties.format), m_Properties.width, m_Properties.height, 0, OpenGLUtils::GetOpenGLFormat(m_Properties.format), GL_UNSIGNED_BYTE, m_Buffer.data());
		glBindTexture(GL_TEXTURE_2D, 0);
#else
		glTextureSubImage2D(m_Handle, 0, 0, 0, m_Properties.width, m_Properties.height, OpenGLUtils::GetOpenGLFormat(m_Properties.format), GL_UNSIGNED_BYTE, data);
#endif
	}

	namespace OpenGLUtils
	{
		uint16_t GetOpenGLFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::SRGBA: return GL_RGBA;
			case ImageFormat::RGBA: return GL_RGBA;
			case ImageFormat::BGRA: return GL_BGRA;
			case ImageFormat::Depth32F: return GL_DEPTH_COMPONENT;
			}

			PX_CORE_ASSERT(false, "Invalid ImageFormat.");
			return GL_INVALID_ENUM;
		}

		uint16_t GetOpenGLInternalFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::SRGBA: return GL_SRGB8_ALPHA8;
			case ImageFormat::RGBA: return GL_RGBA8;
			case ImageFormat::BGRA: return GL_RGBA8;
			case ImageFormat::Depth32F: return GL_DEPTH_COMPONENT32F;
			}

			PX_CORE_ASSERT(false, "Invalid ImageFormat.");
			return GL_INVALID_ENUM;
		}
	}
}
