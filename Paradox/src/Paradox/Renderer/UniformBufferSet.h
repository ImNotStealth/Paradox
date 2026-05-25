#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Renderer/UniformBuffer.h"

namespace Paradox
{
	class PARADOX_API UniformBufferSet
	{
	public:
		~UniformBufferSet() = default;

		virtual Shared<UniformBuffer> GetCurrent() = 0;
		virtual Shared<UniformBuffer> Get(uint32_t frameIndex) = 0;

		static Shared<UniformBufferSet> Create(uint32_t size);
	};
}