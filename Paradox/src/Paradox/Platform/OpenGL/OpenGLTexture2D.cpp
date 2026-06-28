#include "pxpch.h"
#include "OpenGLTexture2D.h"

#include "Paradox/Platform/OpenGL/OpenGL.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

namespace Paradox
{
	OpenGLTexture2D::OpenGLTexture2D(const std::string& debugName, const std::filesystem::path& filePath)
	{
		m_Properties.debugName = debugName;
		Create(filePath);
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureProperties& props, const std::filesystem::path& filePath)
		: m_Properties(props)
	{
		Create(filePath);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_TextureID);
	}

	void OpenGLTexture2D::Create(const std::filesystem::path& filePath)
	{
		int width, height, channels;
		stbi_uc* pixels = stbi_load(filePath.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);

		PX_CORE_ASSERT(pixels, "Failed to load Texture.");

		m_Properties.width = width;
		m_Properties.height = height;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);

#ifdef PX_PLATFORM_PSVITA
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GetOpenGLFormat(m_Properties.format), m_Properties.width, m_Properties.height, 0, GetOpenGLFormat(m_Properties.format), GL_UNSIGNED_BYTE, pixels);
		glBindTexture(GL_TEXTURE_2D, 0);
#else
		glTextureStorage2D(m_TextureID, 1, GetOpenGLInternalFormat(m_Properties.format), m_Properties.width, m_Properties.height);
#endif

		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GetOpenGLFilter(m_Properties.minFilter));
		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GetOpenGLFilter(m_Properties.magFilter));

		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GetOpenGLWrap(m_Properties.wrap));
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GetOpenGLWrap(m_Properties.wrap));

#ifndef PX_PLATFORM_PSVITA
		glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Properties.width, m_Properties.height, GetOpenGLFormat(m_Properties.format), GL_UNSIGNED_BYTE, pixels);
#endif

		stbi_image_free(pixels);

		PX_CORE_TRACE("Created Texture: {0} ({1}x{2})", m_Properties.debugName, m_Properties.width, m_Properties.height);
	}

	uint16_t OpenGLTexture2D::GetOpenGLFormat(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::SRGBA: return GL_RGBA;
		case ImageFormat::RGBA: return GL_RGBA;
		}

		PX_CORE_ASSERT(false, "Invalid TextureFormat.");
		return GL_INVALID_ENUM;
	}

	uint16_t OpenGLTexture2D::GetOpenGLInternalFormat(ImageFormat format)
	{
		switch (format)
		{
		case ImageFormat::SRGBA: return GL_SRGB8_ALPHA8;
		case ImageFormat::RGBA: return GL_RGBA8;
		}

		PX_CORE_ASSERT(false, "Invalid TextureFormat.");
		return GL_INVALID_ENUM;
	}

	uint16_t OpenGLTexture2D::GetOpenGLFilter(TextureFilter filter)
	{
		switch (filter)
		{
		case TextureFilter::Nearest: return GL_NEAREST;
		case TextureFilter::Linear: return GL_LINEAR;
		}

		PX_CORE_ASSERT(false, "Invalid TextureFilter.");
		return GL_INVALID_ENUM;
	}

	uint16_t OpenGLTexture2D::GetOpenGLWrap(TextureWrap wrap)
	{
		switch (wrap)
		{
		case TextureWrap::Repeat: return GL_REPEAT;
		case TextureWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
#ifndef PX_PLATFORM_PSVITA
		case TextureWrap::ClampToBorder: return GL_CLAMP_TO_BORDER;
#endif
		case TextureWrap::ClampToEdge: return GL_CLAMP_TO_EDGE;
		}

		PX_CORE_ASSERT(false, "Invalid TextureWrap.");
		return GL_INVALID_ENUM;
	}
}