#pragma once

#include "Panels/Panel.h"

namespace Paradox
{
	class StatisticsPanel : public Panel
	{
	public:
		StatisticsPanel();

		void OnImGuiRender(bool* opened) override;

	private:
		const class EditorApp& m_AppRef;
	};
}