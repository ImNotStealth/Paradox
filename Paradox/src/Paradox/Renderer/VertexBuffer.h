#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	enum class VertexBufferUsage
	{
		Static = 0,
		Dynamic
	};

	class PARADOX_API VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;
	
		virtual void SetData(const void* data, uint32_t size) = 0;
	
		static Shared<VertexBuffer> Create(uint32_t size, VertexBufferUsage usage);
		static Shared<VertexBuffer> Create(const void* data, uint32_t size, VertexBufferUsage usage); 
	};
}