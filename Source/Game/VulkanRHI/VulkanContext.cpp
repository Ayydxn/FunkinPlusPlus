#include "FunkinPCH.h"
#include "VulkanContext.h"
#include "VulkanDebugUtils.h"
#include "VulkanPlatformUtils.h"

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
    // (Ayydxn) Overlays (like RivaTuner Statistics Server) inject VK_IMAGE_USAGE_STORAGE_BIT, triggering a false positive for this VUID.
    // Silencing this specific ID prevents masking other genuine swapchain configuration errors.
    if (CallbackData && CallbackData->pMessageIdName)
    {
        constexpr std::array<std::string_view, 2> SuppressedMessageIDs
        {
            "VUID-VkSwapchainCreateInfoKHR-imageFormat-01778",
            "VUID-VkImageViewCreateInfo-usage-02275"
        };
        
        if (std::ranges::find(SuppressedMessageIDs, CallbackData->pMessageIdName) != SuppressedMessageIDs.end())
            return vk::False;
    }

    // Setup the message tag
    auto Tag = "Vulkan Validation";
    if (MessageType & vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
    {
        Tag = "Vulkan Validation - Performance";
    }
    else if (MessageType & vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
    {
        Tag = "Vulkan Validation - Validation";
    }

    // Route message based on severity
    if (MessageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
    {
        LOG_ERROR_TAG(Tag, CallbackData->pMessage);
    }
    else if (MessageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
    {
        LOG_WARN_TAG(Tag, CallbackData->pMessage);
    }
    
    return vk::False;
}

bool CVulkanContext::Initialize(uint32 WindowID, const FNativeWindowHandle& NativeWindowHandle, uint32 InitialWindowWidth, uint32 InitialWindowHeight, bool bRequestVSync)
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
    
    // (Ayydxn) A temporary surface that is used only during physical device selection so surface presentation support can be queried.
    const vk::SurfaceKHR ProbeSurface = CVulkanPlatformUtils::CreateSurface(m_Instance, NativeWindowHandle);
    if (!ProbeSurface)
        return false;
    
    m_Device = std::make_shared<CVulkanDevice>(m_Instance, ProbeSurface);
    
    m_Instance.destroySurfaceKHR(ProbeSurface);
    
    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Device->GetLogicalDevice());
    
    const vk::Extent2D InitialSize = { InitialWindowWidth, InitialWindowHeight };
    m_MainWindowSwapChain = std::make_unique<CVulkanSwapChain>(m_Device, m_Instance, WindowID, NativeWindowHandle, InitialSize, 2, bRequestVSync);
    
    return true;
}

void CVulkanContext::Destroy()
{
    m_MainWindowSwapChain->Destroy(m_Instance);
    m_MainWindowSwapChain.reset();
    
    m_Device->Destroy();
    m_Device.reset();
    
    if (bEnableValidationLayers)
        m_Instance.destroyDebugUtilsMessengerEXT(m_DebugUtilsMessenger);
    
    m_Instance.destroy();
}

void CVulkanContext::OnWindowResized(uint32 WindowID, uint32 NewWidth, uint32 NewHeight)
{
    if (!m_MainWindowSwapChain)
        return;
    
    // TODO: (Ayydxn) Transform into a WindowID -> CVulkanSwapChain lookup once secondary windows (like one for the chart editor) are fully supported.
    if (WindowID != m_MainWindowSwapChain->GetWindowID())
        return;
    
    m_MainWindowSwapChain->Resize(NewWidth, NewHeight);
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
