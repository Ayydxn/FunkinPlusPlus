#include "FunkinPCH.h"
#include "VulkanContext.h"
#include "VulkanDebugUtils.h"

#include <SDL3/SDL_vulkan.h>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#ifndef FUNKIN_BUILD_DISTRIBUTION
    bool CVulkanContext::bEnableValidationLayers = true;
#else
    bool CVulkanContext::bEnableValidationLayers = false;
#endif

static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugMessengerCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT MessageSeverity, vk::DebugUtilsMessageTypeFlagsEXT MessageType,
    vk::DebugUtilsMessengerCallbackDataEXT const* CallbackData, void*)
{
    switch (MessageSeverity)
    {
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose: break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo: break;
            
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
        {
            if (MessageType == vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
            {
                LOG_WARN_TAG("Vulkan Validation - Performance", CallbackData->pMessage);
                break;
            }
            
            if (MessageType == vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
            {
                LOG_WARN_TAG("Vulkan Validation - Validation", CallbackData->pMessage);
                break;
            }
        }

        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
        {
            if (MessageType == vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
            {
                LOG_ERROR_TAG("Vulkan Validation - Performance", CallbackData->pMessage);
                break;
            }
            
            if (MessageType == vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
            {
                LOG_ERROR_TAG("Vulkan Validation - Validation", CallbackData->pMessage);
                break;
            }
        }
    }
    
    return vk::False;
}

// NOTE: (Ayydxn) NativeWindowHandle is currently unused, but we'll need this it comes time for surface creation. Might as well as have it from now.
bool CVulkanContext::Initialize(const FNativeWindowHandle& NativeWindowHandle)
{
    VULKAN_HPP_DEFAULT_DISPATCHER.init();
    
    const auto InstanceExtensions = GetRequiredInstanceExtensions();
    const auto AvailableValidationLayers = CVulkanDebugUtils::GetSupportedValidationLayers();
    
    vk::ApplicationInfo ApplicationInfo = {};
    ApplicationInfo.sType = vk::StructureType::eApplicationInfo;
    ApplicationInfo.pApplicationName = "Friday Night Funkin'++";
    ApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    ApplicationInfo.pEngineName = "Friday Night Funkin'++";
    ApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    ApplicationInfo.apiVersion = VK_API_VERSION_1_3;
    
    vk::InstanceCreateInfo InstanceCreateInfo;
    InstanceCreateInfo.sType = vk::StructureType::eInstanceCreateInfo;
    InstanceCreateInfo.pApplicationInfo = &ApplicationInfo;
    InstanceCreateInfo.enabledExtensionCount = static_cast<uint32>(InstanceExtensions.size());
    InstanceCreateInfo.ppEnabledExtensionNames = InstanceExtensions.data();
    InstanceCreateInfo.enabledLayerCount = 0;
    InstanceCreateInfo.ppEnabledLayerNames = nullptr;
    InstanceCreateInfo.pNext = nullptr;
    InstanceCreateInfo.flags = vk::InstanceCreateFlags();
    
    #ifdef FUNKIN_PLATFORM_MAC
        InstanceCreateInfo.flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
    #endif
    
    if (bEnableValidationLayers)
    {
        if (!AvailableValidationLayers.empty())
        {
            vk::DebugUtilsMessengerCreateInfoEXT DebugMessengerCreateInfo;
            PopulateDebugMessengerCreateInfo(DebugMessengerCreateInfo);
            
            InstanceCreateInfo.enabledLayerCount = static_cast<uint32>(AvailableValidationLayers.size());
            InstanceCreateInfo.ppEnabledLayerNames = AvailableValidationLayers.data();
            InstanceCreateInfo.pNext = reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&DebugMessengerCreateInfo);
        }
        else
        {
            LOG_WARN_TAG("VulkanRHI", "No validation layers were found by the Vulkan Loader. Validation will be unavailable. (VkInstance)");
        }
    }
    
    VK_CHECK_RESULT(vk::createInstance(InstanceCreateInfo), m_Instance, "Failed to create Vulkan instance!")
    
    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Instance);
    
    CreateDebugMessenger();
    
    return true;
}

void CVulkanContext::Destroy()
{
    if (bEnableValidationLayers)
        m_Instance.destroyDebugUtilsMessengerEXT(m_DebugUtilsMessenger);
    
    m_Instance.destroy();
}

void CVulkanContext::CreateDebugMessenger()
{
    if (!bEnableValidationLayers)
        return;

    vk::DebugUtilsMessengerCreateInfoEXT DebugMessengerCreateInfo;
    PopulateDebugMessengerCreateInfo(DebugMessengerCreateInfo);
    
    VK_CHECK_RESULT(m_Instance.createDebugUtilsMessengerEXT(DebugMessengerCreateInfo), m_DebugUtilsMessenger, "Failed to create Vulkan debug messenger!")
}

void CVulkanContext::PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& DebugMessengerCreateInfo)
{
    DebugMessengerCreateInfo.sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT;
    DebugMessengerCreateInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
    DebugMessengerCreateInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
    DebugMessengerCreateInfo.pfnUserCallback = DebugMessengerCallback;
    DebugMessengerCreateInfo.pUserData = nullptr;
    DebugMessengerCreateInfo.flags = vk::DebugUtilsMessengerCreateFlagsEXT();
}

std::vector<const char*> CVulkanContext::GetRequiredInstanceExtensions()
{
    uint32_t RequiredInstanceExtensionsCount = 0;
    char const* const* RequiredInstanceExtensions = SDL_Vulkan_GetInstanceExtensions(&RequiredInstanceExtensionsCount);
    
    std::vector<const char*> InstanceExtensions(RequiredInstanceExtensions, RequiredInstanceExtensions + RequiredInstanceExtensionsCount);
    
    if (bEnableValidationLayers)
        InstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    
    #ifdef FUNKIN_PLATFORM_MAC
        InstanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    #endif
    
    return InstanceExtensions;
}
