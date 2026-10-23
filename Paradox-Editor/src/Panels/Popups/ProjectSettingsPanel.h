#pragma once

#include "Panels/Panel.h"

namespace Paradox
{
	class ProjectSettingsPanel : public Panel
	{
	public:
		ProjectSettingsPanel()
			: Panel("Project Settings") {}

		void OnImGuiRender(bool* opened, float deltaTime) override;
	};
}