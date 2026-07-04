#pragma once

#include "Paradox/Renderer/Texture.h"
#include "Paradox/Platform/OpenGL/OpenGLImage.h"

namespace Paradox
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& debugName, const std::filesystem::path& filePath);
		OpenGLTexture2D(const TextureProperties& props, const std::filesystem::path& filePath);
		OpenGLTexture2D(const TextureProperties& props, Shared<Image> image);
		~OpenGLTexture2D();

		uint32_t GetWidth() const override { return m_Properties.width; }
		uint32_t GetHeight() const override { return m_Properties.height; }
		TextureProperties& GetProperties() override { return m_Properties; }
		uint32_t GetTextureID() const { return std::static_pointer_cast<OpenGLImage>(m_Image)->GetHandle(); }
		uint32_t GetSamplerID() const { return m_SamplerID; }

		Shared<Image> GetImage() override { return m_Image; }

	private:
		void Create(const std::filesystem::path& filePath);
		void CreateSampler();
		uint16_t GetOpenGLFilter(TextureFilter filter);
		uint16_t GetOpenGLWrap(TextureWrap wrap);

	private:
		TextureProperties m_Properties;
		uint32_t m_SamplerID = 0;
		Shared<Image> m_Image = nullptr;
	};
}