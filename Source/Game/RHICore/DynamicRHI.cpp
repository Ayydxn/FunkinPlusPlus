#include "FunkinPCH.h"
#include "DynamicRHI.h"
#include "VulkanRHI/VulkanContext.h"
#include "VulkanRHI/VulkanDynamicRHI.h"

std::unique_ptr<IDynamicRHI> CreateDynamicRHI(ERHIBackend RHIBackend, IRHIContext& RHIContext)
{
    switch (RHIBackend)
    {
        case ERHIBackend::OpenGL: verifyFunkinf(false, "Failed to create a dynamic RHI! OpenGL isn't supported!") break;
        case ERHIBackend::Vulkan: return std::make_unique<CVulkanDynamicRHI>(dynamic_cast<CVulkanContext&>(RHIContext));
        case ERHIBackend::Direct3D11: verifyFunkinf(false, "Failed to create a dynamic RHI! DirectX 11 isn't supported!") break;
        case ERHIBackend::Direct3D12: verifyFunkinf(false, "Failed to create a dynamic RHI! DirectX 12 isn't supported!") break;
        case ERHIBackend::Metal: verifyFunkinf(false, "Failed to create a dynamic RHI! Metal isn't supported!") break;
    }
    
    verifyFunkinf(false, "Failed to create a dynamic RHI! An unknown/unsupported dynamic RHI was requested!")
    return nullptr;
}
