#include "pxpch.h"
#include "VulkanShader.h"

#include "Paradox/Platform/Vulkan/VulkanDevice.h"
#include "Paradox/Platform/Vulkan/VulkanBuffer.h"
#include "Paradox/Platform/Vulkan/VulkanUniformBuffer.h"
#include "Paradox/Platform/Vulkan/VulkanUniformBufferSet.h"
#include "Paradox/Platform/Vulkan/VulkanImage.h"
#include "Paradox/Platform/Vulkan/VulkanTexture2D.h"
#include "Paradox/Renderer/Renderer.h"

namespace Paradox
{
    VulkanShader::VulkanShader(const std::string& name, const std::string& vertFilePath, const std::string& fragFilePath)
        : m_Name(name)
    {
        VkShaderModule vertModule = CreateShaderModule(GetBasePath() + vertFilePath + ".spv");
        VkShaderModule fragModule = CreateShaderModule(GetBasePath() + fragFilePath + ".spv");

        m_ShaderStages.reserve(GetStageCount());
        VkPipelineShaderStageCreateInfo vertStageInfo = {};
        vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertStageInfo.module = vertModule;
        vertStageInfo.pName = "main";
        m_ShaderStages.push_back(vertStageInfo);

        VkPipelineShaderStageCreateInfo fragStageInfo = {};
        fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragStageInfo.module = fragModule;
        fragStageInfo.pName = "main";
        m_ShaderStages.push_back(fragStageInfo);

        PX_CORE_TRACE("Created Shader: {0}", name);
    }

    VulkanShader::~VulkanShader()
    {
		VkDevice device = VulkanDevice::Get().GetDevice();
        for (uint32_t i = 0; i < GetStageCount(); i++)
            vkDestroyShaderModule(device, m_ShaderStages[i].module, nullptr);

        if (m_DescriptorSetLayout)
            vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);

        PX_CORE_TRACE("Destroyed Shader: {0}", m_Name);
    }

    void VulkanShader::SetUniformBufferInput(uint32_t binding, Shared<UniformBufferSet> ubo, const std::string& name)
    {
        PX_CORE_ASSERT(m_Inputs.count(binding) == 0, "Duplicate binding.");

        ShaderInput input = {};
        input.type = ShaderInputType::UniformBuffer;
        input.binding = binding;
        input.data[0] = ubo;
        input.name = name;
		m_Inputs[binding] = input;
    }

    void VulkanShader::SetTextureInput(uint32_t binding, Shared<Texture> texture, const std::string& name, uint32_t index)
    {
		if (m_Inputs.count(binding) != 0)
		{
		    ShaderInput& input = m_Inputs[binding];

		    PX_CORE_ASSERT(name == input.name, "Input names must match for readibility.");

		    if (index >= input.data.size())
		        input.data.resize(index + 1);

		    PX_CORE_ASSERT(m_Baked || !input.data[index], "Texture Index already in use.");
		    input.data[index] = texture;
		    return;
		}

        ShaderInput input = {};
        input.type = ShaderInputType::Texture;
        input.binding = binding;
        input.data[index] = texture;
        input.name = name;
        m_Inputs[binding] = input;
    }

    void VulkanShader::BakeInput()
    {
        PX_CORE_ASSERT(!m_Baked, "Shader Inputs already baked.");

        std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
        layoutBindings.reserve(m_Inputs.size());
        for (const auto& [binding, entry] : m_Inputs)
        {
            VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.emplace_back();
            layoutBinding.binding = binding;
            layoutBinding.descriptorType = GetVulkanDescriptorType(entry.type);
            layoutBinding.descriptorCount = entry.data.size();

            //TODO: Set for both vertex and fragment, this is to have parity with OpenGL. Check later on if this is ok or not
            layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
            layoutBinding.pImmutableSamplers = nullptr;
        }

        VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.pBindings = layoutBindings.data();
        layoutCreateInfo.bindingCount = layoutBindings.size();

        VK_CHECK_RESULT(vkCreateDescriptorSetLayout(VulkanDevice::Get().GetDevice(), &layoutCreateInfo, nullptr, &m_DescriptorSetLayout));
        VulkanUtils::SetDebugName(VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, m_DescriptorSetLayout, m_Name + " (Descriptor Set Layout)");

        VulkanDevice::Get().AllocateDescriptorSets(m_DescriptorSetLayout, Renderer::GetMaxFramesInFlight(), m_DescriptorSets);
        
        for (uint32_t i = 0; i < m_DescriptorSets.size(); i++)
            UpdateDirtyInputs(i);
        m_Baked = true;
    }

    void VulkanShader::UpdateDirtyInputs(uint32_t frameIndex)
    {
        std::unordered_map<uint32_t, const ShaderInput*> dirtyInputs;
        
        for (const auto& [binding, entry] : m_Inputs)
        {
            if (entry.type == ShaderInputType::UniformBuffer)
            {
                Shared<VulkanUniformBufferSet> ubo = std::static_pointer_cast<VulkanUniformBufferSet>(entry.data[0]);
                Shared<VulkanBuffer> uboBuffer = std::static_pointer_cast<VulkanUniformBuffer>(ubo->Get(frameIndex))->GetBuffer();

                std::vector<void*>& handles = m_InputHandles[frameIndex][binding];
                if (handles.empty() || uboBuffer->GetBuffer() != handles[0])
                    dirtyInputs[binding] = &entry;
            }
            else if (entry.type == ShaderInputType::Texture)
            {
                std::vector<void*>& handles = m_InputHandles[frameIndex][binding];
                if (handles.size() != entry.data.size())
                    handles.resize(entry.data.size(), nullptr);

                //Currently this rebuilds the entire array if one texture has changed.
                bool dirty = false;
                for (size_t t = 0; t < entry.data.size(); t++)
                {
                    Shared<VulkanTexture2D> texture = std::static_pointer_cast<VulkanTexture2D>(entry.data[t]);
                    Shared<VulkanImage> image = std::static_pointer_cast<VulkanImage>(texture->GetImage());
                    if (image->GetImageView() != handles[t])
                    {
                        dirty = true;
                        break;
                    }
                }

                if (dirty)
                    dirtyInputs[binding] = &entry;
            }
        }

        if (dirtyInputs.empty())
            return;

        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<std::vector<VkDescriptorImageInfo>> imageInfos; // binding, image index
        std::vector<VkWriteDescriptorSet> writes;
        bufferInfos.reserve(dirtyInputs.size());
        imageInfos.reserve(dirtyInputs.size());
        writes.reserve(dirtyInputs.size());

        for (const auto& [binding, entry] : dirtyInputs)
        {
            if (entry->type == ShaderInputType::UniformBuffer)
            {
                Shared<VulkanUniformBufferSet> ubo = std::static_pointer_cast<VulkanUniformBufferSet>(entry->data[0]);
                Shared<VulkanBuffer> uboBuffer = std::static_pointer_cast<VulkanUniformBuffer>(ubo->Get(frameIndex))->GetBuffer();
                VkDescriptorBufferInfo& bufferInfo = bufferInfos.emplace_back();
                bufferInfo.buffer = uboBuffer->GetBuffer();
                bufferInfo.range = uboBuffer->GetSize();
                bufferInfo.offset = 0;

                m_InputHandles[frameIndex][binding] = { uboBuffer->GetBuffer() };

                VkWriteDescriptorSet& write = writes.emplace_back();
                write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.dstSet = m_DescriptorSets[frameIndex];
                write.dstBinding = binding;
                write.descriptorCount = 1;
                write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                write.pBufferInfo = &bufferInfo;
            }
            else if (entry->type == ShaderInputType::Texture)
            {
                //Currently this rebuilds the entire array if one texture has changed.
                std::vector<void*>& handles = m_InputHandles[frameIndex][binding];
                handles.resize(entry->data.size());

                std::vector<VkDescriptorImageInfo>& bindingImageInfos = imageInfos.emplace_back();
                bindingImageInfos.resize(entry->data.size());

                for (size_t t = 0; t < entry->data.size(); t++)
                {
                    Shared<VulkanTexture2D> texture = std::static_pointer_cast<VulkanTexture2D>(entry->data[t]);
                    Shared<VulkanImage> image = std::static_pointer_cast<VulkanImage>(texture->GetImage());

                    bindingImageInfos[t].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    bindingImageInfos[t].sampler = texture->GetSampler();
                    bindingImageInfos[t].imageView = image->GetImageView();

                    handles[t] = image->GetImageView();
                }

                VkWriteDescriptorSet& write = writes.emplace_back();
                write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.dstSet = m_DescriptorSets[frameIndex];
                write.dstBinding = binding;
                write.dstArrayElement = 0;
                write.descriptorCount = entry->data.size();
                write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                write.pImageInfo = bindingImageInfos.data();
            }
        }
        vkUpdateDescriptorSets(VulkanDevice::Get().GetDevice(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
    }

    std::vector<char> VulkanShader::ReadFile(const std::string& filePath)
    {
        std::ifstream file(filePath, std::ios::ate | std::ios::binary);
        PX_CORE_ASSERT(file.is_open(), "Failed to open file, have the SPIR-V binaries been compiled?.");

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }

    VkShaderModule VulkanShader::CreateShaderModule(const std::string& filePath)
    {
        std::vector<char> shaderCode = ReadFile(filePath);

        // This magic number is part of the SPIR-V spec, https://registry.khronos.org/SPIR-V/specs/unified1/SPIRV.html
        uint32_t magic = 0;
        memcpy(&magic, shaderCode.data(), sizeof(uint32_t));
		PX_CORE_ASSERT(magic == 0x07230203, "Shader file is not a SPIR-V binary.");

        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shaderCode.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

        VkShaderModule shaderModule = {};
        VK_CHECK_RESULT(vkCreateShaderModule(VulkanDevice::Get().GetDevice(), &createInfo, nullptr, &shaderModule));
        VulkanUtils::SetDebugName(VK_OBJECT_TYPE_SHADER_MODULE, shaderModule, m_Name + " (Shader Module)");

        return shaderModule;
    }

    VkDescriptorType VulkanShader::GetVulkanDescriptorType(ShaderInputType type)
    {
        switch (type)
        {
		case ShaderInputType::UniformBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case ShaderInputType::Texture: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        }

        PX_CORE_ASSERT(false, "Invalid ShaderInputType.");
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }
}