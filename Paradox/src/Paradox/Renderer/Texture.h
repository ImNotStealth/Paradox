#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Renderer/Image.h"

namespace Paradox
{
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

	struct TextureProperties : public ImageProperties
	{
		TextureWrap wrap = TextureWrap::Repeat;
		TextureFilter minFilter = TextureFilter::Linear;
		TextureFilter magFilter = TextureFilter::Linear;
		bool anisotropicFiltering = true;
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

		virtual Shared<Image> GetImage() = 0;

		static Shared<Texture2D> Create(const std::string& debugName, const std::filesystem::path& filePath);
		static Shared<Texture2D> Create(const TextureProperties& props, const std::filesystem::path& filePath);
		static Shared<Texture2D> CreateFromImage(const TextureProperties& props, Shared<Image> image);
	};
}