#include "pxpch.h"
#include "OpenGLTexture.h"

#include "Paradox/Platform/OpenGL/OpenGL.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

namespace Paradox
{
	OpenGLTexture::OpenGLTexture(const std::string& debugName, const std::filesystem::path& filePath)
	{
		m_Properties.debugName = debugName;
		Create(filePath);
	}

	OpenGLTexture::OpenGLTexture(const TextureProperties& props, const std::filesystem::path& filePath)
		: m_Properties(props)
	{
		Create(filePath);
	}

	OpenGLTexture::~OpenGLTexture()
	{
		glDeleteTextures(1, &m_TextureID);
	}

	void OpenGLTexture::Create(const std::filesystem::path& filePath)
	{
		int width, height, channels;
		stbi_uc* pixels = stbi_load(filePath.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);

		PX_CORE_ASSERT(pixels, "Failed to load Texture.");

		m_Properties.width = width;
		m_Properties.height = height;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
		glTextureStorage2D(m_TextureID, 1, GetOpenGLInternalFormat(m_Properties.format), m_Properties.width, m_Properties.height);

		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GetOpenGLFilter(m_Properties.minFilter));
		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GetOpenGLFilter(m_Properties.magFilter));

		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GetOpenGLWrap(m_Properties.wrap));
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GetOpenGLWrap(m_Properties.wrap));

		glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Properties.width, m_Properties.height, GetOpenGLFormat(m_Properties.format), GL_UNSIGNED_BYTE, pixels);

		stbi_image_free(pixels);

		PX_CORE_TRACE("Created Texture: {0} ({1}x{2})", m_Properties.debugName, m_Properties.width, m_Properties.height);
	}

	uint16_t OpenGLTexture::GetOpenGLFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::SRGBA: return GL_RGBA;
		case TextureFormat::RGBA: return GL_RGBA;
		}

		PX_CORE_ASSERT(false, "Invalid TextureFormat.");
		return GL_INVALID_ENUM;
	}

	uint16_t OpenGLTexture::GetOpenGLInternalFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::SRGBA: return GL_SRGB8_ALPHA8;
		case TextureFormat::RGBA: return GL_RGBA8;
		}

		PX_CORE_ASSERT(false, "Invalid TextureFormat.");
		return GL_INVALID_ENUM;
	}

	uint16_t OpenGLTexture::GetOpenGLFilter(TextureFilter filter)
	{
		switch (filter)
		{
		case TextureFilter::Nearest: return GL_NEAREST;
		case TextureFilter::Linear: return GL_LINEAR;
		}

		PX_CORE_ASSERT(false, "Invalid TextureFilter.");
		return GL_INVALID_ENUM;
	}

	uint16_t OpenGLTexture::GetOpenGLWrap(TextureWrap wrap)
	{
		switch (wrap)
		{
		case TextureWrap::Repeat: return GL_REPEAT;
		case TextureWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
		case TextureWrap::ClampToBorder: return GL_CLAMP_TO_BORDER;
		case TextureWrap::ClampToEdge: return GL_CLAMP_TO_EDGE;
		}

		PX_CORE_ASSERT(false, "Invalid TextureWrap.");
		return GL_INVALID_ENUM;
	}
}