#include "FunkinPCH.h"
#include "GraphicsPipeline.h"
#include "VulkanRHI/VulkanContext.h"
#include "VulkanRHI/VulkanGraphicsPipeline.h"

std::shared_ptr<IGraphicsPipeline> CreateGraphicsPipeline(ERHIBackend RHIBackend, IRHIContext& RHIContext, const IShader& Shader)
{
    switch (RHIBackend)
    {
        case ERHIBackend::OpenGL: verifyFunkinf(false, "Failed to create graphics pipeline! OpenGL isn't supported!") break;
        
        case ERHIBackend::Vulkan:
        {
            const auto& VulkanContext = dynamic_cast<CVulkanContext&>(RHIContext);
            const auto& VulkanShader = dynamic_cast<const CVulkanShader&>(Shader);
            
            return std::make_shared<CVulkanGraphicsPipeline>(VulkanContext, VulkanShader);
        }
        
        case ERHIBackend::Direct3D11: verifyFunkinf(false, "Failed to create graphics pipeline! DirectX 11 isn't supported!") break;
        case ERHIBackend::Direct3D12: verifyFunkinf(false, "Failed to create graphics pipeline! DirectX 12 isn't supported!") break;
        case ERHIBackend::Metal: verifyFunkinf(false, "Failed to create graphics pipeline! Metal isn't supported!") break;
    }
    
    verifyFunkinf(false, "Failed to create graphics pipeline! An unknown/unsupported RHI backend was requested!")
    return nullptr;

}
