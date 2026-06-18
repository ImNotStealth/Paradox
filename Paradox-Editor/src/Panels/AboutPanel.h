#pragma once

#include "Panels/Panel.h"

namespace Paradox
{
	class AboutPanel : public Panel
	{
	public:
		AboutPanel()
			: Panel("About") {}

		void OnImGuiRender() override;
	};
}