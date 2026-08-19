#pragma once

#include "VulkanRHI/VulkanIncludes.h"

#include <vma/vk_mem_alloc.hpp>

struct FAllocatedVulkanBuffer
{
    vk::Buffer Buffer;
    vma::Allocation BufferAllocation;
};

struct FAllocatedVulkanImage
{
    vk::Image Image;
    vma::Allocation ImageAllocation;
};

class CVulkanMemoryAllocator
{
public:
    void Initialize(vk::Instance VulkanInstance, vk::PhysicalDevice PhysicalDevice, vk::Device LogicalDevice);
    void Shutdown() const;
    
    FAllocatedVulkanBuffer AllocateBuffer(const vk::BufferCreateInfo& BufferCreateInfo, vma::MemoryUsage MemoryUsage, vma::AllocationCreateFlags AllocationFlags = {}) const;
    void DestroyBuffer(const FAllocatedVulkanBuffer& AllocatedVulkanBuffer) const;
    
    FAllocatedVulkanImage AllocateImage(const vk::ImageCreateInfo& ImageCreateInfo, vma::MemoryUsage MemoryUsage, vma::AllocationCreateFlags AllocationFlags = {}) const;
    void DestroyImage(const FAllocatedVulkanImage& AllocatedVulkanImage) const;
    
    void* MapMemory(vma::Allocation Allocation) const;
    void UnmapMemory(vma::Allocation Allocation) const;
private:
    vma::Allocator m_Allocator;
};
