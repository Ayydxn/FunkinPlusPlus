#include "FunkinPCH.h"
#include "VertexBuffer.h"
#include "VulkanRHI/VulkanVertexBuffer.h"

std::shared_ptr<IVertexBuffer> CreateVertexBuffer(ERHIBackend RHIBackend, IRHIContext& RHIContext, const FVertexBufferDescription& Description)
{
    switch (RHIBackend)
    {
        case ERHIBackend::OpenGL: verifyFunkinf(false, "Failed to create vertex buffer! OpenGL isn't supported!") break;
        
        case ERHIBackend::Vulkan:
        {
            auto& VulkanContext = dynamic_cast<CVulkanContext&>(RHIContext);
            
            return std::make_shared<CVulkanVertexBuffer>(VulkanContext, Description);
        }
        
        case ERHIBackend::Direct3D11: verifyFunkinf(false, "Failed to create vertex buffer! DirectX 11 isn't supported!") break;
        case ERHIBackend::Direct3D12: verifyFunkinf(false, "Failed to create vertex buffer! DirectX 12 isn't supported!") break;
        case ERHIBackend::Metal: verifyFunkinf(false, "Failed to create vertex buffer! Metal isn't supported!") break;
    }
    
    verifyFunkinf(false, "Failed to create vertex buffer! An unknown/unsupported RHI backend was requested!")
    return nullptr;
}
