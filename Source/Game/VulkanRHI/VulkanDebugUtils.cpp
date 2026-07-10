#include "FunkinPCH.h"
#include "VulkanDebugUtils.h"
#include "VulkanIncludes.h"

std::vector<const char*> CVulkanDebugUtils::GetSupportedValidationLayers()
{
    const auto AvailableInstanceLayers = vk::enumerateInstanceLayerProperties();
    
    const std::initializer_list<const char*> PreferredValidationLayers = { "VK_LAYER_KHRONOS_validation" };
    const std::initializer_list<const char*> LunarGValidationLayers = { "VK_LAYER_LUNARG_standard_validation" };
    const std::initializer_list<const char*> GoogleValidationLayers = { "VK_LAYER_GOOGLE_threading", "VK_LAYER_LUNARG_parameter_validation", "VK_LAYER_LUNARG_object_tracker",
        "VK_LAYER_LUNARG_core_validation", "VK_LAYER_GOOGLE_unique_objects" };

    std::vector<const char*> AvailableValidationLayers = {};
    
    for (const std::initializer_list<const char*>& ValidationLayers : { PreferredValidationLayers, LunarGValidationLayers, GoogleValidationLayers })
    {
        bool bFoundValidationLayers = false;

        for (const char* ValidationLayer : ValidationLayers)
        {
            bFoundValidationLayers = false;
            
            for (const vk::LayerProperties& InstanceLayer : AvailableInstanceLayers.value)
            {
                if (strcmp(InstanceLayer.layerName, ValidationLayer) == 0)
                {
                    bFoundValidationLayers = true;
                    break;
                }
            }

            if (!bFoundValidationLayers)
                break;
        }

        if (bFoundValidationLayers)
        {
            AvailableValidationLayers.reserve(ValidationLayers.size());
            
            for (const char* ValidationLayer : ValidationLayers)
                AvailableValidationLayers.push_back(ValidationLayer);

            break;
        }
    }
    
    return AvailableValidationLayers;
}
