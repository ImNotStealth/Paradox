#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox
{
	class PARADOX_API ImGuiRenderer
	{
	public:
		virtual	~ImGuiRenderer() = default;

		virtual void Init() = 0;
		virtual void Shutdown() = 0;
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void* GetContext() = 0;

		static Unique<ImGuiRenderer> Create();
	};
}