#pragma once

#include "Panels/Panel.h"
#include <Paradox/Renderer/Texture.h>

namespace Paradox
{
	class AboutPanel : public Panel
	{
	public:
		AboutPanel();

		void OnImGuiRender(bool* opened) override;
		
	private:
		Shared<Texture2D> m_LogoTexture = nullptr;
	};
}