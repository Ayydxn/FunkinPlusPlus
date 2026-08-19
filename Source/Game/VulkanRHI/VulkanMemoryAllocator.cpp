#include "FunkinPCH.h"
#include "VulkanMemoryAllocator.h"
#include "VulkanDebugUtils.h"

void CVulkanMemoryAllocator::Initialize(vk::Instance VulkanInstance, vk::PhysicalDevice PhysicalDevice, vk::Device LogicalDevice)
{
    const auto VulkanFunctions = vma::functionsFromDispatcher();
    
    vma::AllocatorCreateInfo AllocatorCreateInfo = {};
    AllocatorCreateInfo.instance = VulkanInstance;
    AllocatorCreateInfo.physicalDevice = PhysicalDevice;
    AllocatorCreateInfo.device = LogicalDevice;
    AllocatorCreateInfo.pVulkanFunctions = &VulkanFunctions;
    AllocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    
    VK_CHECK_RESULT_VOID(vma::createAllocator(&AllocatorCreateInfo, &m_Allocator), "Failed to create Vulkan Memory Allocator!")
}

void CVulkanMemoryAllocator::Shutdown() const
{
    m_Allocator.destroy();
}

FAllocatedVulkanBuffer CVulkanMemoryAllocator::AllocateBuffer(const vk::BufferCreateInfo& BufferCreateInfo, vma::MemoryUsage MemoryUsage,
    vma::AllocationCreateFlags AllocationFlags) const
{
    assertFunkinf(BufferCreateInfo.size > 0, "Cannot allocate a zero-sized Vulkan buffer!")
    
    vma::AllocationCreateInfo AllocationCreateInfo;
    AllocationCreateInfo.usage = MemoryUsage;
    AllocationCreateInfo.flags = AllocationFlags;
    
    std::pair<vma::Allocation, vk::Buffer> Result;
    VK_CHECK_RESULT(m_Allocator.createBuffer(BufferCreateInfo, AllocationCreateInfo), Result, "Failed to allocate Vulkan buffer!")
    
    return {
        .Buffer = Result.second,
        .BufferAllocation = Result.first
    };
}

void CVulkanMemoryAllocator::DestroyBuffer(const FAllocatedVulkanBuffer& AllocatedVulkanBuffer) const
{
    m_Allocator.destroyBuffer(AllocatedVulkanBuffer.Buffer, AllocatedVulkanBuffer.BufferAllocation);
}

FAllocatedVulkanImage CVulkanMemoryAllocator::AllocateImage(const vk::ImageCreateInfo& ImageCreateInfo, vma::MemoryUsage MemoryUsage,
    vma::AllocationCreateFlags AllocationFlags) const
{
    assertFunkinf(ImageCreateInfo.extent.width > 0 && ImageCreateInfo.extent.height > 0, "Cannot allocate a zero-sized Vulkan image!")
    
    vma::AllocationCreateInfo AllocationCreateInfo;
    AllocationCreateInfo.usage = MemoryUsage;
    AllocationCreateInfo.flags = AllocationFlags;
    
    std::pair<vma::Allocation, vk::Image> Result;
    VK_CHECK_RESULT(m_Allocator.createImage(ImageCreateInfo, AllocationCreateInfo), Result, "Failed to allocate Vulkan image!")
    
    return {
        .Image = Result.second,
        .ImageAllocation = Result.first
    };
}

void CVulkanMemoryAllocator::DestroyImage(const FAllocatedVulkanImage& AllocatedVulkanImage) const
{
    m_Allocator.destroyImage(AllocatedVulkanImage.Image, AllocatedVulkanImage.ImageAllocation);
}

void* CVulkanMemoryAllocator::MapMemory(vma::Allocation Allocation) const
{
    void* Result = nullptr;
    VK_CHECK_RESULT(m_Allocator.mapMemory(Allocation), Result, "Failed to map VMA allocation!")
    
    return Result;
}

void CVulkanMemoryAllocator::UnmapMemory(vma::Allocation Allocation) const
{
    m_Allocator.unmapMemory(Allocation);
}
