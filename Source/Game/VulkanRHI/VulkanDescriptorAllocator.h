#pragma once

#include "VulkanDevice.h"
#include "VulkanIncludes.h"

struct FVulkanDescriptorPoolSizes
{
    uint32 MaxSets = 64;
    std::vector<vk::DescriptorPoolSize> DescriptorPoolSizes =
    {
        { vk::DescriptorType::eUniformBuffer, 64 },
        { vk::DescriptorType::eStorageBuffer, 32 },
        { vk::DescriptorType::eCombinedImageSampler, 64 },
        { vk::DescriptorType::eSampledImage, 32 },
        { vk::DescriptorType::eStorageImage, 16 },
        { vk::DescriptorType::eSampler, 16 }
    };
};

class CVulkanDescriptorAllocator
{
public:
    CVulkanDescriptorAllocator(CVulkanDevice& VulkanDevice, uint32 FramesInFlight, const FVulkanDescriptorPoolSizes& PoolSizes = {});
    ~CVulkanDescriptorAllocator();
    
    CVulkanDescriptorAllocator(const CVulkanDescriptorAllocator&) = delete;
    CVulkanDescriptorAllocator& operator=(const CVulkanDescriptorAllocator&) = delete;
    
    // Allocates a single transient descriptor set shaped by the provided descriptor set layout from the descriptor pool of the specified frame index.
    // The returned set is only valid until the next call to Reset. It is never freed individually.
    vk::DescriptorSet Allocate(uint32 FrameIndex, vk::DescriptorSetLayout DescriptorSetLayout) const;
    
    // Resets the pool belonging to FrameIndex, invalidating (and reclaiming) every VkDescriptorSet allocated from it since the last time this frame slot was reset.
    // Must only be called once the frame's in-flight fence has been waited on.
    void Reset(uint32 FrameIndex) const;
private:
    void CreateDescriptorPools();
private:
    CVulkanDevice& m_VulkanDevice;
    FVulkanDescriptorPoolSizes m_PoolSizes;
    
    std::vector<vk::DescriptorPool> m_DescriptorPools;
    uint32 m_FramesInFlight;
};
