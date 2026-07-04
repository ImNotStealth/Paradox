#pragma once

#include "Paradox/Renderer/ImGuiRenderer.h"
#include "Paradox/Platform/Vulkan/VulkanRenderPass.h"

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
		Shared<VulkanRenderPass> m_RenderPass;
	};
}