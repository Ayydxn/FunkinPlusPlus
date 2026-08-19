#include "FunkinPCH.h"
#include "IndexBuffer.h"
#include "VulkanRHI/VulkanIndexBuffer.h"

std::shared_ptr<IIndexBuffer> CreateIndexBuffer(ERHIBackend RHIBackend, IRHIContext& RHIContext, const FIndexBufferDescription& Description)
{
    switch (RHIBackend)
    {
        case ERHIBackend::OpenGL: verifyFunkinf(false, "Failed to create index buffer! OpenGL isn't supported!") break;
        
        case ERHIBackend::Vulkan:
        {
            auto& VulkanContext = dynamic_cast<CVulkanContext&>(RHIContext);
            
            return std::make_shared<CVulkanIndexBuffer>(VulkanContext, Description);
        }
        
        case ERHIBackend::Direct3D11: verifyFunkinf(false, "Failed to create index buffer! DirectX 11 isn't supported!") break;
        case ERHIBackend::Direct3D12: verifyFunkinf(false, "Failed to create index buffer! DirectX 12 isn't supported!") break;
        case ERHIBackend::Metal: verifyFunkinf(false, "Failed to create index buffer! Metal isn't supported!") break;
    }
    
    verifyFunkinf(false, "Failed to create index buffer! An unknown/unsupported RHI backend was requested!")
    return nullptr;
}

