#include "FunkinPCH.h"
#include "RHIContext.h"
#include "VulkanRHI/VulkanContext.h"

std::unique_ptr<IRHIContext> CreateRHIContext(ERHIBackend RHIBackend)
{
    switch (RHIBackend)
    {
        case ERHIBackend::OpenGL: verifyFunkinf(false, "Failed to create a RHI context! OpenGL isn't supported!")
        case ERHIBackend::Vulkan: return std::make_unique<CVulkanContext>();
        case ERHIBackend::Direct3D11: verifyFunkinf(false, "Failed to create a RHI context! DirectX 11 isn't supported!")
        case ERHIBackend::Direct3D12: verifyFunkinf(false, "Failed to create a RHI context! DirectX 12 isn't supported!")
        case ERHIBackend::Metal: verifyFunkinf(false, "Failed to create a RHI context! Metal isn't supported!")
    }
    
    verifyFunkinf(false, "Failed to create a RHI context! An unknown/unsupported RHI backend was requested!")
    return nullptr;
}

std::string GetRHIBackendName(ERHIBackend RHIBackend)
{
    switch (RHIBackend)
    {
        case ERHIBackend::OpenGL: return "OpenGL";
        case ERHIBackend::Vulkan: return "Vulkan";
        case ERHIBackend::Direct3D11: return "DirectX 11";
        case ERHIBackend::Direct3D12: return "DirectX 12";
        case ERHIBackend::Metal: return "Metal";
    }

    verifyFunkinf(false, "Failed to get string for unknown/unsupported RHI backend!")
    return "Unknown RHI Backend";
}
