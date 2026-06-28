#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	enum class ImageFormat
	{
		SRGBA,
		RGBA,
		BGRA,
		Depth32F
	};

	enum class ImageUsage
	{
		Texture,
		Attachment
	};

	struct ImageProperties
	{
		uint32_t width = 0, height = 0;
		ImageFormat format = ImageFormat::RGBA;
		ImageUsage usage = ImageUsage::Attachment;
		std::string debugName;
	};

	class Image
	{
	public:
		virtual ~Image() = default;

		virtual uint32_t GetWidth() = 0;
		virtual uint32_t GetHeight() = 0;
		virtual ImageFormat GetFormat() = 0;
		virtual ImageProperties& GetProperties() = 0;

		virtual void SetData(void* data, uint32_t size) = 0;
		virtual void* GetData() = 0;
		virtual uint32_t GetDataSize() = 0;

		static Shared<Image> Create(const ImageProperties& props);
	};
}