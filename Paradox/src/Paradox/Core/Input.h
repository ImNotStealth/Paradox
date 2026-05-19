#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Core/KeyCodes.h"

namespace Paradox
{
	class PARADOX_API Input
	{
	public:
		static bool IsKeyPressed(KeyCode keyCode);
	};
}