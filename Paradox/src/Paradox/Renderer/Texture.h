#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	enum TextureFormat
	{
		SRGBA,
		RGBA
	};

	enum TextureWrap
	{
		Repeat,
		MirroredRepeat,
		ClampToBorder,
		ClampToEdge
	};

	enum TextureFilter
	{
		Nearest,
		Linear
	};

	struct TextureProperties
	{
		// Width and Height are overwritten when loading the texture.
		uint32_t width = 0, height = 0;
		TextureFormat format = TextureFormat::RGBA;
		TextureWrap wrap = TextureWrap::Repeat;
		TextureFilter minFilter = TextureFilter::Linear;
		TextureFilter magFilter = TextureFilter::Linear;
		bool anisotropicFiltering = true;

		std::string debugName;
	};

	class PARADOX_API Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual TextureProperties& GetProperties() = 0;
	};

	class PARADOX_API Texture2D : public Texture
	{
	public:
		virtual ~Texture2D() = default;

		static Shared<Texture2D> Create(const std::string& debugName, const std::filesystem::path& filePath);
		static Shared<Texture2D> Create(const TextureProperties& props, const std::filesystem::path& filePath);
	};
}