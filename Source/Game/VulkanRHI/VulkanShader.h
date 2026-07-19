#pragma once

#include "VulkanDevice.h"
#include "Renderer/Shaders/ShaderCompiler.h"
#include "RHICore/Shader.h"
#include "VulkanRHI/VulkanIncludes.h"

class CVulkanShader final : public IShader
{
public:
    CVulkanShader(CVulkanDevice& VulkanDevice, CShaderCompiler& ShaderCompiler, const std::string& Name, const std::string& Source);
    CVulkanShader(CVulkanDevice& VulkanDevice, CShaderCompiler& ShaderCompiler, const std::filesystem::path& Filepath);
    ~CVulkanShader() override;
    
    std::vector<vk::PipelineShaderStageCreateInfo> GetStageCreateInfos() const;
private:
    void CreateShaderModules(const std::unordered_map<SlangStage, FCompiledShaderStage>& CompiledStages);
private:
    struct FShaderModuleStage
    {
        vk::ShaderModule ShaderModule;
        vk::ShaderStageFlagBits ShaderStageFlag;
        std::string EntryPointName;
    };

    std::vector<FShaderModuleStage> m_ShaderModuleStages;

    CVulkanDevice& m_VulkanDevice;
};