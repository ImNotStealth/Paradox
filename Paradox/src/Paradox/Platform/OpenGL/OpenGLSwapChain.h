#pragma once

#include "Paradox/Renderer/SwapChain.h"

namespace Paradox
{
	class OpenGLSwapChain : public SwapChain
	{
	public:
		void Init(Window* window) override {}
		void Create(uint32_t width, uint32_t height, bool vsync) override {}
		void OnResize(uint32_t width, uint32_t height) override;
		void RequestResize() override;

		void Begin() override {}
		void End() override;

	private:
		bool m_ResizeRequested = false;
	};
}