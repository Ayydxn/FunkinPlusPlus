#include "FunkinPCH.h"
#include "UniformBuffer.h"
#include "VulkanRHI/VulkanUniformBuffer.h"

std::shared_ptr<IUniformBuffer> CreateUniformBuffer(ERHIBackend RHIBackend, IRHIContext& RHIContext, const FUniformBufferDescription& Description)
{
    switch (RHIBackend)
    {
        case ERHIBackend::OpenGL: verifyFunkinf(false, "Failed to create uniform buffer! OpenGL isn't supported!") break;
        
        case ERHIBackend::Vulkan:
        {
            auto& VulkanContext = dynamic_cast<CVulkanContext&>(RHIContext);
            
            return std::make_shared<CVulkanUniformBuffer>(VulkanContext, Description);
        }
        
        case ERHIBackend::Direct3D11: verifyFunkinf(false, "Failed to create uniform buffer! DirectX 11 isn't supported!") break;
        case ERHIBackend::Direct3D12: verifyFunkinf(false, "Failed to create uniform buffer! DirectX 12 isn't supported!") break;
        case ERHIBackend::Metal: verifyFunkinf(false, "Failed to create uniform buffer! Metal isn't supported!") break;
    }
    
    verifyFunkinf(false, "Failed to create uniform buffer! An unknown/unsupported RHI backend was requested!")
    return nullptr;
}
