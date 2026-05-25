#include "pxpch.h"
#include "VulkanUniformBufferSet.h"

#include "Paradox/Renderer/Renderer.h"
#include "Paradox/Core/Application.h"
#include "Paradox/Platform/Vulkan/VulkanSwapChain.h"

namespace Paradox
{
	VulkanUniformBufferSet::VulkanUniformBufferSet(uint32_t size)
	{
		uint32_t framesInFlight = Renderer::GetMaxFramesInFlight();
		m_UniformBuffers.resize(framesInFlight);

		for (uint32_t i = 0; i < framesInFlight; i++)
			m_UniformBuffers[i] = UniformBuffer::Create(size);
	}

	Shared<UniformBuffer> VulkanUniformBufferSet::GetCurrent()
	{
		Shared<VulkanSwapChain> swapchain = std::static_pointer_cast<VulkanSwapChain>(Application::Get().GetWindow().GetSwapChain());
		return m_UniformBuffers[swapchain->GetCurrentFrameIndex()];
	}
}