#pragma once

#include "Panels/Panel.h"

namespace Paradox
{
	class PackageProjectPanel : public Panel
	{
	public:
		PackageProjectPanel()
			: Panel("Package Project") {}

		void OnImGuiRender(bool* opened, float deltaTime) override;

	private:
		void Package();

	private:
		std::thread m_CompileThread;
		std::atomic<bool> m_IsCompiling = false;
	};
}