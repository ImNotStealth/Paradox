#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	enum class IndexBufferUsage
	{
		Static = 0,
		Dynamic
	};

	class PARADOX_API IndexBuffer
	{
	public:
		~IndexBuffer() = default;

		virtual void SetData(const void* data, uint32_t count) = 0;
		virtual uint32_t GetCount() const = 0;

		static Shared<IndexBuffer> Create(const void* data, uint32_t count, IndexBufferUsage usage);
		static Shared<IndexBuffer> Create(uint32_t count, IndexBufferUsage usage);
	};
}