#include "FunkinPCH.h"
#include "Shader.h"
#include "RHIContext.h"
#include "VulkanRHI/VulkanContext.h"
#include "VulkanRHI/VulkanShader.h"

std::shared_ptr<IShader> CreateShader(ERHIBackend RHIBackend, IRHIContext& RHIContext, CShaderCompiler& ShaderCompiler, const std::string& Name, const std::string& Source)
{
    switch (RHIBackend)
    {
        case ERHIBackend::OpenGL: verifyFunkinf(false, "Failed to create shader! OpenGL isn't supported!") break;
        case ERHIBackend::Vulkan:
        {
            const auto& VulkanContext = dynamic_cast<CVulkanContext&>(RHIContext);
            
            return std::make_shared<CVulkanShader>(VulkanContext.GetDevice(), ShaderCompiler, Name, Source);
        }
        case ERHIBackend::Direct3D11: verifyFunkinf(false, "Failed to create shader! DirectX 11 isn't supported!") break;
        case ERHIBackend::Direct3D12: verifyFunkinf(false, "Failed to create shader! DirectX 12 isn't supported!") break;
        case ERHIBackend::Metal: verifyFunkinf(false, "Failed to create shader! Metal isn't supported!") break;
    }
    
    verifyFunkinf(false, "Failed to create shader! An unknown/unsupported RHI backend was requested!")
    return nullptr;
}

std::shared_ptr<IShader> CreateShader(ERHIBackend RHIBackend, IRHIContext& RHIContext, CShaderCompiler& ShaderCompiler, const std::filesystem::path& Filepath)
{
    switch (RHIBackend)
    {
        case ERHIBackend::OpenGL: verifyFunkinf(false, "Failed to create shader! OpenGL isn't supported!") break;
        case ERHIBackend::Vulkan:
        {
            const auto& VulkanContext = dynamic_cast<CVulkanContext&>(RHIContext);
            
            return std::make_shared<CVulkanShader>(VulkanContext.GetDevice(), ShaderCompiler, Filepath);
        }
        case ERHIBackend::Direct3D11: verifyFunkinf(false, "Failed to create shader! DirectX 11 isn't supported!") break;
        case ERHIBackend::Direct3D12: verifyFunkinf(false, "Failed to create shader! DirectX 12 isn't supported!") break;
        case ERHIBackend::Metal: verifyFunkinf(false, "Failed to create shader! Metal isn't supported!") break;
    }
    
    verifyFunkinf(false, "Failed to create shader! An unknown/unsupported RHI backend was requested!")
    return nullptr;
}
