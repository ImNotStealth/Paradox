#include "pxpch.h"
#include "Renderer.h"

namespace Paradox
{
	Unique<RendererAPI> Renderer::s_RenderAPI = nullptr;
	
	void Renderer::Init()
	{
		s_RenderAPI = RendererAPI::Create();
		s_RenderAPI->Init();
	}
}