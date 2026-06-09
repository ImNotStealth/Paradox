#pragma once

#include "Paradox/Core/Base.h"

#include <vulkan/vulkan.h>

namespace Paradox::VulkanUtils
{
	void LoadDebugFunctions();
	void SetDebugName(VkObjectType objectType, const void* objectHandle, const std::string& name);
	const char* VkResultToString(VkResult result);
}

#ifdef PX_ENABLE_ASSERTS
	#define VK_CHECK_RESULT(f)\
	{\
		VkResult result = (f);\
		if (result != VK_SUCCESS)\
		{\
			PX_CORE_CRITICAL("VkResult check failed. Got: '{0}'", ::Paradox::VulkanUtils::VkResultToString(result));\
			PX_CORE_ASSERT(false);\
		}\
	}
#else
	#define VK_CHECK_RESULT(f) (f)
#endif
