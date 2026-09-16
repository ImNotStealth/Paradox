#pragma once

#include "Panels/Panel.h"

namespace Paradox
{
	class AssetIndexPanel : public Panel
	{
	public:
		AssetIndexPanel();

		void OnImGuiRender(bool* opened) override;
	};
}