#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	class PARADOX_API UniformBuffer
	{
	public:
		~UniformBuffer() = default;

		virtual void SetData(const void* data, uint32_t size) = 0;

		static Shared<UniformBuffer> Create(uint32_t size);
	};
}