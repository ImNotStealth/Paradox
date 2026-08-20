#pragma once

#include "Panels/Panel.h"
#include <Paradox/Renderer/Texture.h>

namespace Paradox
{
	class AboutPanel : public Panel
	{
	public:
		AboutPanel()
			: Panel("About")
		{
			m_LogoTexture = Texture2D::Create("Paradox Logo", "Assets/Textures/Paradox_Full_256x.png");
		}

		void OnImGuiRender() override;
		
	private:
		Shared<Texture2D> m_LogoTexture = nullptr;
	};
}