#include "pxpch.h"
#include "VulkanImGuiRenderer.h"

#include "Paradox/Core/Application.h"
#include "Paradox/Renderer/Renderer.h"
#include "Paradox/Platform/Vulkan/Vulkan.h"
#include "Paradox/Platform/Vulkan/VulkanDevice.h"
#include "Paradox/Platform/Vulkan/VulkanContext.h"
#include "Paradox/Platform/Vulkan/VulkanSwapChain.h"
#include "Paradox/Platform/Vulkan/VulkanRenderPass.h"
#include "Paradox/Platform/Vulkan/VulkanImage.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

namespace Paradox
{
	void VulkanImGuiRenderer::Init()
	{
		VkDevice device = VulkanDevice::Get().GetDevice();

		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE },
			{ VK_DESCRIPTOR_TYPE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE },
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		for (VkDescriptorPoolSize& pool_size : pool_sizes)
			pool_info.maxSets += pool_size.descriptorCount;
		pool_info.poolSizeCount = (uint32_t)IM_COUNTOF(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		VK_CHECK_RESULT(vkCreateDescriptorPool(device, &pool_info, nullptr, &m_DescriptorPool));
		VulkanUtils::SetDebugName(VK_OBJECT_TYPE_DESCRIPTOR_POOL, m_DescriptorPool, "ImGui Descriptor Pool");

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
		ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)Application::Get().GetWindow().GetHandle(), true);

		Shared<VulkanSwapChain> swapChain = std::static_pointer_cast<VulkanSwapChain>(Application::Get().GetWindow().GetSwapChain());

		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance = VulkanContext::GetVkInstance();
		initInfo.PhysicalDevice = VulkanDevice::Get().GetPhysicalDevice();
		initInfo.Device = device;
		initInfo.QueueFamily = VulkanDevice::Get().GetQueueFamilyIndices().graphicsFamily;
		initInfo.Queue = VulkanDevice::Get().GetGraphicsQueue();
		initInfo.DescriptorPool = m_DescriptorPool;
		initInfo.MinImageCount = 2;
		initInfo.ImageCount = swapChain->GetImageCount();

		RenderPassProperties renderPassProps = {};
		renderPassProps.clearColor = false;
		renderPassProps.swapchainTarget = true;
		renderPassProps.attachments = { VulkanUtils::GetImageFormat(swapChain->GetColorFormat()) };
		renderPassProps.debugName = "ImGui RenderPass";
		m_RenderPass = CreateShared<VulkanRenderPass>(renderPassProps);

		ImGui_ImplVulkan_PipelineInfo pipelineInfo = {};
		pipelineInfo.RenderPass = m_RenderPass->GetRenderPass();

		initInfo.PipelineInfoMain = pipelineInfo;
		IM_ASSERT(initInfo.MinImageCount >= 2);

		ImGui_ImplVulkan_Init(&initInfo);
	}

	void VulkanImGuiRenderer::Shutdown()
	{
		VkDevice device = VulkanDevice::Get().GetDevice();
		VK_CHECK_RESULT(vkDeviceWaitIdle(device));
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
	}

	void VulkanImGuiRenderer::BeginFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void VulkanImGuiRenderer::EndFrame()
	{
		ImGui::Render();

		Shared<VulkanSwapChain> swapChain = std::static_pointer_cast<VulkanSwapChain>(Application::Get().GetWindow().GetSwapChain());

		uint32_t commandBufferIndex = swapChain->GetCurrentFrameIndex();
		VkCommandBuffer drawCommandBuffer = swapChain->GetCommandBuffer(commandBufferIndex);

		VkRenderPassBeginInfo passBeginInfo = {};
		passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		passBeginInfo.renderPass = m_RenderPass->GetRenderPass();
		passBeginInfo.framebuffer = swapChain->GetCurrentFramebuffer();
		passBeginInfo.renderArea.extent = swapChain->GetExtent();

		vkCmdBeginRenderPass(drawCommandBuffer, &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), drawCommandBuffer);
		vkCmdEndRenderPass(drawCommandBuffer);

		ImGuiIO& io = ImGui::GetIO();
		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			VK_CHECK_RESULT(vkQueueWaitIdle(VulkanDevice::Get().GetGraphicsQueue()));
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void* VulkanImGuiRenderer::GetContext()
	{
		return ImGui::GetCurrentContext();
	}
}