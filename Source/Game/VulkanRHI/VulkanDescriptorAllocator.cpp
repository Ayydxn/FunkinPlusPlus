#include "FunkinPCH.h"
#include "VulkanDescriptorAllocator.h"
#include "VulkanDebugUtils.h"

CVulkanDescriptorAllocator::CVulkanDescriptorAllocator(CVulkanDevice& VulkanDevice, uint32 FramesInFlight, const FVulkanDescriptorPoolSizes& PoolSizes)
    : m_VulkanDevice(VulkanDevice), m_PoolSizes(PoolSizes), m_FramesInFlight(FramesInFlight)
{
    CreateDescriptorPools();
}

CVulkanDescriptorAllocator::~CVulkanDescriptorAllocator()
{
    const vk::Device LogicalDevice = m_VulkanDevice.GetLogicalDevice();
    
    for (const vk::DescriptorPool DescriptorPool : m_DescriptorPools)
        LogicalDevice.destroyDescriptorPool(DescriptorPool);
}

vk::DescriptorSet CVulkanDescriptorAllocator::Allocate(uint32 FrameIndex, vk::DescriptorSetLayout DescriptorSetLayout) const
{
    verifyFunkinf(FrameIndex < m_DescriptorPools.size(), "Attempted to allocate from a Vulkan descriptor pool with an out-of-range frame index! ({})", FrameIndex)
    
    vk::DescriptorSetAllocateInfo DescriptorSetAllocateInfo = {};
    DescriptorSetAllocateInfo.sType = vk::StructureType::eDescriptorSetAllocateInfo;
    DescriptorSetAllocateInfo.descriptorPool = m_DescriptorPools[FrameIndex];
    DescriptorSetAllocateInfo.descriptorSetCount = 1;
    DescriptorSetAllocateInfo.pSetLayouts = &DescriptorSetLayout;
    
    // We only take a single vk::DescriptorSet, not the whole vector, since we only ever allocate one set at a time here.
    // The pool itself is what's shared/ring-buffered across a frame's many allocations, not individual allocation calls.
    std::vector<vk::DescriptorSet> DescriptorSets;
    VK_CHECK_RESULT(m_VulkanDevice.GetLogicalDevice().allocateDescriptorSets(DescriptorSetAllocateInfo), DescriptorSets,
        "Failed to allocate a Vulkan descriptor set for frame index {}!", FrameIndex)
    
    return DescriptorSets[0];
}

void CVulkanDescriptorAllocator::Reset(uint32 FrameIndex) const
{
    verifyFunkinf(FrameIndex < m_DescriptorPools.size(), "Attempted to reset a Vulkan descriptor pool with an out-of-range frame index! ({})", FrameIndex)
    
    VK_CHECK_RESULT_VOID(m_VulkanDevice.GetLogicalDevice().resetDescriptorPool(m_DescriptorPools[FrameIndex]), "Failed to reset Vulkan descriptor pool for frame index {}!",
        FrameIndex)
}

void CVulkanDescriptorAllocator::CreateDescriptorPools()
{
    const vk::Device LogicalDevice = m_VulkanDevice.GetLogicalDevice();
    
    // We deliberately omit eFreeDescriptorSet since sets are reclaimed in bulk via vkResetDescriptorPool and aren't freed individually.
    // Therefore, the pool doesn't need to support per-set frees.
    vk::DescriptorPoolCreateInfo DescriptorPoolCreateInfo = {};
    DescriptorPoolCreateInfo.sType = vk::StructureType::eDescriptorPoolCreateInfo;
    DescriptorPoolCreateInfo.maxSets = m_PoolSizes.MaxSets;
    DescriptorPoolCreateInfo.poolSizeCount = static_cast<uint32>(m_PoolSizes.DescriptorPoolSizes.size());
    DescriptorPoolCreateInfo.pPoolSizes = m_PoolSizes.DescriptorPoolSizes.data();
    DescriptorPoolCreateInfo.flags = vk::DescriptorPoolCreateFlags();
    
    m_DescriptorPools.reserve(m_FramesInFlight);
    
    for (uint32 FrameIndex = 0; FrameIndex < m_FramesInFlight; ++FrameIndex)
    {
        vk::DescriptorPool DescriptorPool;
        VK_CHECK_RESULT(LogicalDevice.createDescriptorPool(DescriptorPoolCreateInfo), DescriptorPool, "Failed to create Vulkan descriptor pool for frame index {}!",
            FrameIndex)
        
        m_DescriptorPools.push_back(DescriptorPool);
    }
}
