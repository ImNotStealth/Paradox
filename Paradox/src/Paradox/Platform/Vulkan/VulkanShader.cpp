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
        ShaderInput input = {};
        input.type = ShaderInputType::UniformBuffer;
        input.binding = binding;
        input.data = ubo;
        input.name = name;
		m_Inputs[binding] = input;
    }

    void VulkanShader::SetTextureInput(uint32_t binding, Shared<Texture> texture, const std::string& name)
    {
        ShaderInput input = {};
        input.type = ShaderInputType::Texture;
        input.binding = binding;
        input.data = texture;
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
            layoutBinding.descriptorCount = 1;

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
        
        UpdateDirtyInputs();
        m_Baked = true;
    }

    void VulkanShader::UpdateDirtyInputs()
    {
        std::unordered_map<uint32_t, std::unordered_map<uint32_t, const ShaderInput*>> dirtyInputs;
        for (uint32_t i = 0; i < m_DescriptorSets.size(); i++)
        {
            for (const auto& [binding, entry] : m_Inputs)
            {
                if (entry.type == ShaderInputType::UniformBuffer)
                {
                    Shared<VulkanUniformBufferSet> ubo = std::static_pointer_cast<VulkanUniformBufferSet>(entry.data);
                    Shared<VulkanBuffer> uboBuffer = std::static_pointer_cast<VulkanUniformBuffer>(ubo->Get(i))->GetBuffer();

                    if (uboBuffer->GetBuffer() != m_InputHandles[i][binding])
                        dirtyInputs[i][binding] = &entry;
                }
                else if (entry.type == ShaderInputType::Texture)
                {
                    Shared<VulkanTexture2D> texture = std::static_pointer_cast<VulkanTexture2D>(entry.data);
                    Shared<VulkanImage> image = std::static_pointer_cast<VulkanImage>(texture->GetImage());

                    if (image->GetImageView() != m_InputHandles[i][binding])
						dirtyInputs[i][binding] = &entry;
                }
            }
        }

        if (dirtyInputs.empty())
            return;

        for (const auto& [i, bindings] : dirtyInputs)
        {
            std::vector<VkDescriptorBufferInfo> bufferInfos;
            std::vector<VkDescriptorImageInfo> imageInfos;
            std::vector<VkWriteDescriptorSet> writes;
            bufferInfos.reserve(bindings.size());
            imageInfos.reserve(bindings.size());
            writes.reserve(bindings.size());

            for (const auto& [binding, entry] : bindings)
            {
                if (entry->type == ShaderInputType::UniformBuffer)
                {
                    Shared<VulkanUniformBufferSet> ubo = std::static_pointer_cast<VulkanUniformBufferSet>(entry->data);
                    Shared<VulkanBuffer> uboBuffer = std::static_pointer_cast<VulkanUniformBuffer>(ubo->Get(i))->GetBuffer();
                    VkDescriptorBufferInfo& bufferInfo = bufferInfos.emplace_back();
                    bufferInfo.buffer = uboBuffer->GetBuffer();
                    bufferInfo.range = uboBuffer->GetSize();
                    bufferInfo.offset = 0;

                    m_InputHandles[i][binding] = uboBuffer->GetBuffer();

                    VkWriteDescriptorSet& write = writes.emplace_back();
                    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    write.dstSet = m_DescriptorSets[i];
                    write.dstBinding = binding;
                    write.descriptorCount = 1;
                    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    write.pBufferInfo = &bufferInfo;
                }
                else if (entry->type == ShaderInputType::Texture)
                {
                    Shared<VulkanTexture2D> texture = std::static_pointer_cast<VulkanTexture2D>(entry->data);
                    Shared<VulkanImage> image = std::static_pointer_cast<VulkanImage>(texture->GetImage());
                    VkDescriptorImageInfo& imageInfo = imageInfos.emplace_back();
                    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    imageInfo.sampler = texture->GetSampler();
                    imageInfo.imageView = image->GetImageView();

                    m_InputHandles[i][binding] = image->GetImageView();

                    VkWriteDescriptorSet& write = writes.emplace_back();
                    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    write.dstSet = m_DescriptorSets[i];
                    write.dstBinding = binding;
                    write.descriptorCount = 1;
                    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    write.pImageInfo = &imageInfo;
                }
            }
            vkUpdateDescriptorSets(VulkanDevice::Get().GetDevice(), writes.size(), writes.data(), 0, nullptr);
        }
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