#pragma once

#include "VulkanIncludes.h"
#include "Application/Window.h"
#include "RHICore/RHIContext.h"

class CVulkanContext final : public IRHIContext
{
public:
    CVulkanContext() = default;
    ~CVulkanContext() override = default;
    
    bool Initialize(const FNativeWindowHandle& NativeWindowHandle) override;
    void Destroy() override;
private:
    void CreateDebugMessenger();
    void PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& DebugMessengerCreateInfo);
    
    std::vector<const char*> GetRequiredInstanceExtensions();
private:
    static bool bEnableValidationLayers;
    
    vk::Instance m_Instance;
    vk::DebugUtilsMessengerEXT m_DebugUtilsMessenger;
};
