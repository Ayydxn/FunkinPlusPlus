#include "FunkinPCH.h"
#include "Texture.h"
#include "VulkanRHI/VulkanTexture.h"

std::shared_ptr<ITexture> CreateTexture(ERHIBackend RHIBackend, IRHIContext& RHIContext, const FTextureDescription& Description)
{
    switch (RHIBackend)
    {
        case ERHIBackend::OpenGL: verifyFunkinf(false, "Failed to create texture! OpenGL isn't supported!") break;
        
        case ERHIBackend::Vulkan:
        {
            auto& VulkanContext = dynamic_cast<CVulkanContext&>(RHIContext);
            
            return std::make_shared<CVulkanTexture>(VulkanContext, Description);
        }
        
        case ERHIBackend::Direct3D11: verifyFunkinf(false, "Failed to create texture! DirectX 11 isn't supported!") break;
        case ERHIBackend::Direct3D12: verifyFunkinf(false, "Failed to create texture! DirectX 12 isn't supported!") break;
        case ERHIBackend::Metal: verifyFunkinf(false, "Failed to create texture! Metal isn't supported!") break;
    }
    
    verifyFunkinf(false, "Failed to create texture! An unknown/unsupported RHI backend was requested!")
    return nullptr;
}

std::shared_ptr<ITexture> CreateTexture(ERHIBackend RHIBackend, IRHIContext& RHIContext, const std::filesystem::path& Filepath)
{
    switch (RHIBackend)
    {
        case ERHIBackend::OpenGL: verifyFunkinf(false, "Failed to create texture! OpenGL isn't supported!") break;
        
        case ERHIBackend::Vulkan:
        {
            auto& VulkanContext = dynamic_cast<CVulkanContext&>(RHIContext);
            
            return std::make_shared<CVulkanTexture>(VulkanContext, Filepath);
        }
        
        case ERHIBackend::Direct3D11: verifyFunkinf(false, "Failed to create texture! DirectX 11 isn't supported!") break;
        case ERHIBackend::Direct3D12: verifyFunkinf(false, "Failed to create texture! DirectX 12 isn't supported!") break;
        case ERHIBackend::Metal: verifyFunkinf(false, "Failed to create texture! Metal isn't supported!") break;
    }
    
    verifyFunkinf(false, "Failed to create texture! An unknown/unsupported RHI backend was requested!")
    return nullptr;
}

uint32 GetTextureFormatBytesPerPixel(ETextureFormat Format)
{
    switch (Format)
    {
        case ETextureFormat::None: break;
        case ETextureFormat::RGBA8: return 4;
    }
    
    verifyFunkinf(false, "Failed to get the bytes per pixel of an unknown/undefined texture format!")
    return 0;
}
