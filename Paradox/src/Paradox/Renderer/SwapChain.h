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
		virtual void RequestResize() = 0;
		virtual void SetVSync(bool enabled) = 0;

		virtual void Begin() = 0;
		virtual void End() = 0;

		static Shared<SwapChain> Create();
	};
}