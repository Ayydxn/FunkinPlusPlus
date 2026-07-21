#include "FunkinPCH.h"
#include "VulkanShader.h"
#include "VulkanDebugUtils.h"

namespace
{
    vk::ShaderStageFlagBits ConvertSlangStageToVulkan(const SlangStage SlangStage)
    {
        switch (SlangStage)
        {
            case SLANG_STAGE_VERTEX: return vk::ShaderStageFlagBits::eVertex;
            case SLANG_STAGE_FRAGMENT: return vk::ShaderStageFlagBits::eFragment;
            
            default:
            {
                verifyFunkinf(false, "Failed to convert Slang shader stage to a Vulkan shader stage!")
                return static_cast<vk::ShaderStageFlagBits>(-1);
            }
        }
    }
}

CVulkanShader::CVulkanShader(CVulkanDevice& VulkanDevice, CShaderCompiler& ShaderCompiler, const std::string& Name, const std::string& Source)
    : m_VulkanDevice(VulkanDevice)
{
    m_Name = Name;
    
    std::unordered_map<SlangStage, FCompiledShaderStage> CompiledShaderStages;
    ShaderCompiler.CompileShader(Name, Source, CompiledShaderStages);
 
    CreateShaderModules(CompiledShaderStages);
}

CVulkanShader::CVulkanShader(CVulkanDevice& VulkanDevice, CShaderCompiler& ShaderCompiler, const std::filesystem::path& Filepath)
    : m_VulkanDevice(VulkanDevice)
{
    m_Name = Filepath.stem().string();
    
    std::unordered_map<SlangStage, FCompiledShaderStage> CompiledShaderStages;
    ShaderCompiler.CompileShaderFromFile(Filepath, CompiledShaderStages);
 
    CreateShaderModules(CompiledShaderStages);
}

CVulkanShader::~CVulkanShader()
{
    for (const FShaderModuleStage& ShaderModuleStage : m_ShaderModuleStages)
        m_VulkanDevice.GetLogicalDevice().destroyShaderModule(ShaderModuleStage.ShaderModule);
}

std::vector<vk::PipelineShaderStageCreateInfo> CVulkanShader::GetStageCreateInfos() const
{
    std::vector<vk::PipelineShaderStageCreateInfo> PipelineShaderStageCreateInfos;
    PipelineShaderStageCreateInfos.reserve(m_ShaderModuleStages.size());
 
    for (const auto& [ShaderModule, ShaderStageFlag, EntryPointName] : m_ShaderModuleStages)
    {
        vk::PipelineShaderStageCreateInfo PipelineShaderStageCreateInfo;
        PipelineShaderStageCreateInfo.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
        PipelineShaderStageCreateInfo.stage = ShaderStageFlag;
        PipelineShaderStageCreateInfo.module = ShaderModule;
        PipelineShaderStageCreateInfo.pName = "main";
        PipelineShaderStageCreateInfo.flags = vk::PipelineShaderStageCreateFlags();
        
        PipelineShaderStageCreateInfos.push_back(PipelineShaderStageCreateInfo);
    }
 
    return PipelineShaderStageCreateInfos;
}

void CVulkanShader::CreateShaderModules(const std::unordered_map<SlangStage, FCompiledShaderStage>& CompiledStages)
{
    m_ShaderModuleStages.reserve(CompiledStages.size());
 
    for (const auto& [SlangShaderStage, CompiledShaderStage] : CompiledStages)
    {
        vk::ShaderModuleCreateInfo ShaderModuleCreateInfo;
        ShaderModuleCreateInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
        ShaderModuleCreateInfo.codeSize = CompiledShaderStage.Bytecode.size() * sizeof(uint32);
        ShaderModuleCreateInfo.pCode = CompiledShaderStage.Bytecode.data();
        ShaderModuleCreateInfo.flags = vk::ShaderModuleCreateFlags();
 
        vk::ShaderModule ShaderModule;
        VK_CHECK_RESULT(m_VulkanDevice.GetLogicalDevice().createShaderModule(ShaderModuleCreateInfo), ShaderModule,
            "Failed to create Vulkan shader module for shader '{}', stage '{}'!", m_Name, static_cast<uint32>(SlangShaderStage))
 
        FShaderModuleStage ShaderModuleStage;
        ShaderModuleStage.ShaderModule = ShaderModule;
        ShaderModuleStage.ShaderStageFlag = ConvertSlangStageToVulkan(SlangShaderStage);
        ShaderModuleStage.EntryPointName = CompiledShaderStage.EntryPointName;
 
        m_ShaderModuleStages.push_back(std::move(ShaderModuleStage));
    }
}
