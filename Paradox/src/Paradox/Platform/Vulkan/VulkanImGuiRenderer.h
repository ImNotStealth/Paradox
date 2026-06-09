#pragma once

#include "Paradox/Renderer/ImGuiRenderer.h"
#include "Paradox/Renderer/RenderPass.h"

#include <backends/imgui_impl_vulkan.h>


namespace Paradox
{
	class VulkanImGuiRenderer : public ImGuiRenderer
	{
	public:
		void Init() override;
		void Shutdown() override;
		void BeginFrame() override;
		void EndFrame() override;
		void* GetContext() override;

	private:
		VkDescriptorPool m_DescriptorPool;
		Shared<RenderPass> m_RenderPass;
	};
}