#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Core/KeyCodes.h"

#include <glm/glm.hpp>

namespace Paradox
{
	class PARADOX_API Input
	{
	public:
		enum class CursorMode
		{
			Default,
			Hidden,
			Locked
		};

		static bool IsKeyPressed(KeyCode keyCode);
		static bool IsMousePressed(KeyCode keyCode);
		static glm::vec2 GetMousePos();
		static void SetMouseCursorMode(CursorMode mode);
	};
}