#pragma once

#include "Paradox/Core/Base.h"

namespace Paradox {

	class PARADOX_API Application
	{
	public:
		Application();
		virtual ~Application() {}

		virtual void Run() {}
	};

	Application* CreateApplication();
}