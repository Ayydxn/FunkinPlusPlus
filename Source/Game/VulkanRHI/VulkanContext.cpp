#include "FunkinPCH.h"
#include "VulkanContext.h"
#include "VulkanDebugUtils.h"
#include "VulkanPlatformUtils.h"

#include <SDL3/SDL_vulkan.h>

#include <ranges>

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
    
    m_MainWindowID = WindowID;
    
    return RegisterWindow(WindowID, NativeWindowHandle, InitialWindowWidth, InitialWindowHeight, bRequestVSync);
}

void CVulkanContext::Destroy()
{
    m_Device->WaitIdle();
    
    for (auto& SwapChain : m_WindowSwapChains | std::views::values)
        SwapChain->Destroy(m_Instance);
    
    m_WindowSwapChains.clear();
    
    m_Device->Destroy();
    m_Device.reset();
    
    if (bEnableValidationLayers)
        m_Instance.destroyDebugUtilsMessengerEXT(m_DebugUtilsMessenger);
    
    m_Instance.destroy();
}

bool CVulkanContext::RegisterWindow(uint32 WindowID, const FNativeWindowHandle& NativeWindowHandle, uint32 InitialWidth, uint32 InitialHeight, bool bRequestVSync)
{
    verifyFunkinf(!m_WindowSwapChains.contains(WindowID), "Attempted to register window ID {} with the Vulkan context, but it is already registered!", WindowID)
    
    const vk::Extent2D InitialExtent(InitialWidth, InitialHeight);
    auto SwapChain = std::make_unique<CVulkanSwapChain>(m_Device, m_Instance, WindowID, NativeWindowHandle, InitialExtent, DefaultFramesInFlight, bRequestVSync);
    
    m_WindowSwapChains.emplace(WindowID, std::move(SwapChain));
    
    return true;
}

void CVulkanContext::UnregisterWindow(uint32 WindowID)
{
    const auto Iterator = m_WindowSwapChains.find(WindowID);
    if (Iterator == m_WindowSwapChains.end())
        return;
    
    // GPU may still be executing work that references this window's swapchain and its sync resources. So, we need to wait for it.
    m_Device->WaitIdle();
    
    Iterator->second->Destroy(m_Instance);
    m_WindowSwapChains.erase(Iterator);
}

void CVulkanContext::OnWindowResized(uint32 WindowID, uint32 NewWidth, uint32 NewHeight)
{
    CVulkanSwapChain* SwapChain = GetSwapChain(WindowID);
    if (!SwapChain)
        return;
    
    SwapChain->Resize(NewWidth, NewHeight);
}

CVulkanSwapChain* CVulkanContext::GetSwapChain(uint32 WindowID) const
{
    const auto Iterator = m_WindowSwapChains.find(WindowID);
    
    return Iterator != m_WindowSwapChains.end() ? Iterator->second.get() : nullptr;
}

std::vector<uint32> CVulkanContext::GetOrderedWindowIDs() const
{
    std::vector<uint32> OrderedWindowIDs;
    OrderedWindowIDs.reserve(m_WindowSwapChains.size());
    
    if (m_WindowSwapChains.contains(m_MainWindowID))
        OrderedWindowIDs.push_back(m_MainWindowID);
    
    for (const auto& WindowID : m_WindowSwapChains | std::views::keys)
    {
        if (WindowID != m_MainWindowID)
            OrderedWindowIDs.push_back(WindowID);
    }
    
    return OrderedWindowIDs;
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
