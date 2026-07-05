#pragma once

#include "Panels/Panel.h"

namespace Paradox
{
	class AboutPanel : public Panel
	{
	public:
		AboutPanel()
			: Panel("About")
		{
			//m_LogoTexture = Texture2D::Create("Paradox Logo", "Assets/Paradox.png");
		}

		void OnImGuiRender() override;
		
	// Not drawing it for now, crashes when panel closes on Vulkan
	//private:
	//	Shared<Texture2D> m_LogoTexture = nullptr;
	};
}