#pragma once

#include "Paradox/Renderer/SwapChain.h"
#include "Paradox/Platform/Vulkan/VulkanRenderPass.h"

namespace Paradox
{
	class VulkanSwapChain : public SwapChain
	{
	public:
		struct VulkanImage
		{
			VkImage image;
			VkImageView imageView;
		};

		~VulkanSwapChain();

		void Init(Window* window) override;
		void Create(uint32_t width, uint32_t height, bool vsync) override;
		void OnResize(uint32_t width, uint32_t height) override;
		void RequestResize() override;
		void SetVSync(bool enabled) override;

		void Begin() override;
		void End() override;

		VkSurfaceKHR GetSurface() { return m_Surface; }
		VkSwapchainKHR GetSwapChain() { return m_SwapChain; }
		VkExtent2D GetExtent() { return m_Extent; }
		VkFormat GetColorFormat() { return m_ColorFormat; }
		uint32_t GetImageCount() { return m_ImageCount; }

		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }

		// Check if needed
		Shared<RenderPass> GetSwapChainRenderPass() { return m_RenderPass; }
		VkFramebuffer& GetFramebuffer(uint32_t index) { return m_Framebuffers[index]; }
		VkFramebuffer& GetCurrentFramebuffer() { return m_Framebuffers[m_CurrentImage]; }
		VkCommandBuffer& GetCommandBuffer(uint32_t index) { return m_CommandBuffers[index]; }
		VkFence& GetCurrentInFlightFence() { return m_InFlightFences[m_CurrentFrame]; }
		VulkanImage& GetImage(uint32_t index) { return m_Images[index]; }

		uint32_t GetCurrentImageIndex() { return m_CurrentImage; }
		uint32_t GetCurrentFrameIndex() { return m_CurrentFrame; }

	private:
		void FindFormatAndColorSpace();
		VkPresentModeKHR GetPresentMode();

	private:
		bool m_VSync = false;
		uint32_t m_Width = 0, m_Height = 0;
		bool m_ResizeRequested = false;
		
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE, m_OldSwapChain = VK_NULL_HANDLE;
		VkFormat m_ColorFormat;
		VkColorSpaceKHR m_ColorSpace;
		VkExtent2D m_Extent;
		uint32_t m_ImageCount = 0;
		uint32_t m_CurrentFrame = 0;
		uint32_t m_CurrentImage = 0;
		std::vector<VulkanImage> m_Images;
		std::vector<VkFramebuffer> m_Framebuffers;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;
		Shared<RenderPass> m_RenderPass;
	};
}