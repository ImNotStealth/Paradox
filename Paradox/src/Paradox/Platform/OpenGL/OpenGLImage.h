#pragma once

#include "Paradox/Renderer/Image.h"

namespace Paradox
{
	class OpenGLImage : public Image
	{
	public:
		OpenGLImage(const ImageProperties& props);
		~OpenGLImage();

		void Resize(uint32_t width, uint32_t height) override;

		uint32_t GetWidth() override { return m_Properties.width; }
		uint32_t GetHeight() override { return m_Properties.height; }
		ImageFormat GetFormat() override { return m_Properties.format; }
		ImageProperties& GetProperties() override { return m_Properties; }

		void SetData(void* data, uint32_t size) override;
		void* GetData() override { return m_Buffer.data(); }
		uint32_t GetDataSize() override { return !m_Buffer.empty() ? m_Buffer.size() : 0; }

		bool operator==(const Image& other) const override { return m_Handle == ((OpenGLImage&)other).GetHandle(); }
		
		uint32_t GetHandle() const { return m_Handle; }

	private:
		ImageProperties m_Properties;
		uint32_t m_Handle = 0;
		std::vector<uint8_t> m_Buffer;
	};

	namespace OpenGLUtils
	{
		uint16_t GetOpenGLFormat(ImageFormat format);
		uint16_t GetOpenGLInternalFormat(ImageFormat format);
	}
}