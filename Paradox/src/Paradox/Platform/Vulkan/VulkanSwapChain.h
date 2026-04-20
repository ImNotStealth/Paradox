#pragma once

#include "Paradox/Renderer/SwapChain.h"

#include <vulkan/vulkan.h>

namespace Paradox
{
	// PARADOX_API is Temporary
	class PARADOX_API VulkanSwapChain : public SwapChain
	{
	public:
		~VulkanSwapChain();

		void Init(Window* window) override;
		void Create(uint32_t width, uint32_t height, bool vsync) override;
		void OnResize(uint32_t width, uint32_t height) override;

		VkSurfaceKHR GetSurface() { return m_Surface; }
		VkSwapchainKHR GetSwapChain() { return m_SwapChain; }
		VkExtent2D GetExtent() { return m_Extent; }
		VkFormat GetColorFormat() { return m_ColorFormat; }
		uint32_t GetImageCount() { return m_ImageCount; }

		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }

	
		// Temp: Move back to private
		struct VulkanImage
		{
			VkImage image;
			VkImageView imageView;
		};
		std::vector<VulkanImage>& GetImages() { return m_Images; }

	private:
		void FindFormatAndColorSpace();
		VkPresentModeKHR GetPresentMode();

	private:


		bool m_VSync = false;
		uint32_t m_Width = 0, m_Height = 0;
		bool m_FramebufferResized = false;
		
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE, m_OldSwapChain = VK_NULL_HANDLE;
		VkFormat m_ColorFormat;
		VkColorSpaceKHR m_ColorSpace;
		VkExtent2D m_Extent;
		std::vector<VulkanImage> m_Images;
		uint32_t m_ImageCount = 0;
	};
}