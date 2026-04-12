#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	class Window;

	class SwapChain
	{
	public:
		virtual ~SwapChain() = default;

		virtual void Init(Window* window) = 0;
		virtual void Create(uint32_t width, uint32_t height, bool vsync) = 0;
		virtual void OnResize(uint32_t width, uint32_t height) = 0;

		static Shared<SwapChain> Create();
	};
}