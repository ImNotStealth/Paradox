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

	OpenGLTexture2D::OpenGLTexture2D(const TextureProperties& props, Shared<Image> image)
		: m_Properties(props), m_Image(image)
	{
		m_Properties.width = image->GetWidth();
		m_Properties.height = image->GetHeight();
		m_Properties.format = image->GetFormat();
		CreateSampler();
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteSamplers(1, &m_SamplerID);
	}

	void OpenGLTexture2D::Create(const std::filesystem::path& filePath)
	{
		int width, height, channels;
		stbi_uc* pixels = stbi_load(filePath.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);

		PX_CORE_ASSERT(pixels, "Failed to load Texture.");

		uint32_t size = width * height * 4;

		m_Properties.width = width;
		m_Properties.height = height;

		ImageProperties imageProps = {};
		imageProps.width = width;
		imageProps.height = height;
		imageProps.format = m_Properties.format;
		imageProps.usage = ImageUsage::Texture;
		imageProps.debugName = m_Properties.debugName;
		m_Image = Image::Create(imageProps);
		m_Image->SetData(pixels, size);

		CreateSampler();

		stbi_image_free(pixels);

		PX_CORE_TRACE("Created Texture: {0} ({1}x{2})", m_Properties.debugName, m_Properties.width, m_Properties.height);
	}

	void OpenGLTexture2D::CreateSampler()
	{
		glCreateSamplers(1, &m_SamplerID);

		glSamplerParameteri(m_SamplerID, GL_TEXTURE_MIN_FILTER, GetOpenGLFilter(m_Properties.minFilter));
		glSamplerParameteri(m_SamplerID, GL_TEXTURE_MAG_FILTER, GetOpenGLFilter(m_Properties.magFilter));

		glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_S, GetOpenGLWrap(m_Properties.wrap));
		glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_T, GetOpenGLWrap(m_Properties.wrap));
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