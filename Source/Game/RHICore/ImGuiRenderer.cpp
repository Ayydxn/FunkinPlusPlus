#include "FunkinPCH.h"
#include "ImGuiRenderer.h"
#include "VulkanRHI/VulkanImGuiRenderer.h"

IImGuiRenderer* CreateImGuiRenderer(ERHIBackend RHIBackend, IRHIContext& RHIContext)
{
    switch (RHIBackend)
    {
        case ERHIBackend::OpenGL: verifyFunkinf(false, "Failed to create ImGui renderer! OpenGL isn't supported!") break;
        
        case ERHIBackend::Vulkan:
        {
            auto& VulkanContext = dynamic_cast<CVulkanContext&>(RHIContext);
            
            return new CVulkanImGuiRenderer(VulkanContext);
        }
        
        case ERHIBackend::Direct3D11: verifyFunkinf(false, "Failed to create ImGui renderer! DirectX 11 isn't supported!") break;
        case ERHIBackend::Direct3D12: verifyFunkinf(false, "Failed to create ImGui renderer! DirectX 12 isn't supported!") break;
        case ERHIBackend::Metal: verifyFunkinf(false, "Failed to create ImGui renderer! Metal isn't supported!") break;
    }
    
    verifyFunkinf(false, "Failed to create ImGui renderer! An unknown/unsupported RHI backend was requested!")
    return nullptr;
}
