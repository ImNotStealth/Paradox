#pragma once

#include "Paradox/Renderer/Texture.h"

namespace Paradox
{
	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture(const std::string& debugName, const std::filesystem::path& filePath);
		OpenGLTexture(const TextureProperties& props, const std::filesystem::path& filePath);
		~OpenGLTexture();

		uint32_t GetWidth() const override { return m_Properties.width; }
		uint32_t GetHeight() const override { return m_Properties.height; }
		TextureProperties& GetProperties() override { return m_Properties; }
		uint32_t GetTextureID() const { return m_TextureID; }

	private:
		void Create(const std::filesystem::path& filePath);
		uint16_t GetOpenGLFormat(TextureFormat format);
		uint16_t GetOpenGLInternalFormat(TextureFormat format);
		uint16_t GetOpenGLFilter(TextureFilter filter);
		uint16_t GetOpenGLWrap(TextureWrap wrap);

	private:
		TextureProperties m_Properties;
		uint32_t m_TextureID = 0;
	};
}