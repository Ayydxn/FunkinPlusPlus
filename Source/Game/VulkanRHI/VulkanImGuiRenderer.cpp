#include "FunkinPCH.h"
#include "VulkanImGuiRenderer.h"
#include "VulkanDebugUtils.h"
#include "ImGui/ImGuiCommon.h"

#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>

CVulkanImGuiRenderer::CVulkanImGuiRenderer(CVulkanContext& VulkanContext)
    : m_VulkanContext(VulkanContext) {}

void CVulkanImGuiRenderer::Initialize(const FNativeWindowHandle& NativeWindowHandle)
{
    CImGuiCommon::CreateContext();
    
    CreateDescriptorPool();
    
    ImGui_ImplSDL3_InitForVulkan(NativeWindowHandle.SDLWindow);
    
    const auto VulkanDevice = m_VulkanContext.GetDevice();
    const auto VulkanSwapChain = m_VulkanContext.GetSwapChain();
    
    const vk::Format SwapChainColorFormat = VulkanSwapChain->GetImageFormat();
    
    vk::PipelineRenderingCreateInfo PipelineRenderingCreateInfo = {};
    PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    PipelineRenderingCreateInfo.pColorAttachmentFormats = &SwapChainColorFormat;
    
    ImGui_ImplVulkan_InitInfo ImGuiVulkanInitInfo = {};
    ImGuiVulkanInitInfo.ApiVersion = VK_API_VERSION_1_3;
    ImGuiVulkanInitInfo.Instance = m_VulkanContext.GetInstance();
    ImGuiVulkanInitInfo.PhysicalDevice = VulkanDevice.GetPhysicalDevice();
    ImGuiVulkanInitInfo.Device = VulkanDevice.GetLogicalDevice();
    ImGuiVulkanInitInfo.QueueFamily = VulkanDevice.GetQueueFamilyIndices().GraphicsFamily.value();
    ImGuiVulkanInitInfo.Queue = VulkanDevice.GetGraphicsQueue();
    ImGuiVulkanInitInfo.PipelineCache = nullptr;
    ImGuiVulkanInitInfo.DescriptorPool = m_DescriptorPool;
    ImGuiVulkanInitInfo.MinImageCount = VulkanSwapChain->GetMinImageCount();
    ImGuiVulkanInitInfo.ImageCount = VulkanSwapChain->GetImageCount();
    ImGuiVulkanInitInfo.Allocator = nullptr;
    ImGuiVulkanInitInfo.UseDynamicRendering = true;
    ImGuiVulkanInitInfo.PipelineInfoMain.PipelineRenderingCreateInfo = PipelineRenderingCreateInfo;
    ImGuiVulkanInitInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    ImGuiVulkanInitInfo.CheckVkResultFn = [](VkResult Result)
    {
        VK_CHECK_RESULT_VOID(static_cast<vk::Result>(Result), "A Vulkan call from ImGui's Vulkan backend has failed!")
    };
    
    ImGui_ImplVulkan_LoadFunctions(VK_API_VERSION_1_3, [](const char* FunctionName, void* UserData)
    {
        return VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr(static_cast<VkInstance>(UserData), FunctionName);
    }, m_VulkanContext.GetInstance());
    ImGui_ImplVulkan_Init(&ImGuiVulkanInitInfo);
}

void CVulkanImGuiRenderer::Shutdown()
{
    m_VulkanContext.GetDevice().WaitIdle();
    
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    CImGuiCommon::DestroyContext();
    
    m_VulkanContext.GetDevice().GetLogicalDevice().destroyDescriptorPool(m_DescriptorPool);
}

void CVulkanImGuiRenderer::BeginFrame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void CVulkanImGuiRenderer::EndFrame(void* CommandBuffer)
{
    ImGui::Render();
    
    if (ImDrawData* DrawData = ImGui::GetDrawData())
    {
        if (DrawData->DisplaySize.x > 0.0f || DrawData->DisplaySize.y > 0.0f)
            ImGui_ImplVulkan_RenderDrawData(DrawData, static_cast<VkCommandBuffer>(CommandBuffer));
    }
    
    ImGuiIO& ImGuiConfig = ImGui::GetIO();
    if (ImGuiConfig.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void CVulkanImGuiRenderer::CreateDescriptorPool()
{
    // (Ayydxn) Sizes taken from Dear ImGui's own Vulkan example - generous enough for font atlas + a handful of
    // user textures without needing to grow. Revisit if debug UI starts binding many dynamic textures.
    constexpr std::array<vk::DescriptorPoolSize, 2> DescriptorPoolSizes =
    {
        vk::DescriptorPoolSize { vk::DescriptorType::eSampledImage, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE },
        vk::DescriptorPoolSize { vk::DescriptorType::eSampler, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE  }
    };
    
    vk::DescriptorPoolCreateInfo DescriptorPoolCreateInfo = {};
    DescriptorPoolCreateInfo.sType = vk::StructureType::eDescriptorPoolCreateInfo;
    DescriptorPoolCreateInfo.maxSets = 0;
    
    for (const vk::DescriptorPoolSize& DescriptorPoolSize : DescriptorPoolSizes)
        DescriptorPoolCreateInfo.maxSets += DescriptorPoolSize.descriptorCount;
    
    DescriptorPoolCreateInfo.poolSizeCount = static_cast<uint32>(DescriptorPoolSizes.size());
    DescriptorPoolCreateInfo.pPoolSizes = DescriptorPoolSizes.data();
    DescriptorPoolCreateInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    
    VK_CHECK_RESULT(m_VulkanContext.GetDevice().GetLogicalDevice().createDescriptorPool(DescriptorPoolCreateInfo), m_DescriptorPool,
        "Failed to create ImGui's Vulkan descriptor pool!")
}
