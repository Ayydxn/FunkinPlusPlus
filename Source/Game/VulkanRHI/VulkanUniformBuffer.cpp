#include "FunkinPCH.h"
#include "VulkanUniformBuffer.h"

CVulkanUniformBuffer::CVulkanUniformBuffer(CVulkanContext& VulkanContext, const FUniformBufferDescription& Description)
    : m_VulkanContext(VulkanContext)
{
    m_SizeInBytes = Description.SizeInBytes;
    
    const CVulkanMemoryAllocator& MemoryAllocator = VulkanContext.GetMemoryAllocator();
    
    vk::BufferCreateInfo BufferCreateInfo = {};
    BufferCreateInfo.sType = vk::StructureType::eBufferCreateInfo;
    BufferCreateInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer;
    BufferCreateInfo.size = Description.SizeInBytes;
    BufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;
    
    m_AllocatedUniformBuffer = MemoryAllocator.AllocateBuffer(BufferCreateInfo, vma::MemoryUsage::eAutoPreferHost,
        vma::AllocationCreateFlagBits::eHostAccessSequentialWrite);
    
    if (Description.InitialData)
        SetData(Description.InitialData, Description.SizeInBytes);
}

CVulkanUniformBuffer::~CVulkanUniformBuffer()
{
    const CVulkanMemoryAllocator& MemoryAllocator = m_VulkanContext.GetMemoryAllocator();
    
    MemoryAllocator.DestroyBuffer(m_AllocatedUniformBuffer);
}

void CVulkanUniformBuffer::SetData(const void* Data, uint64 SizeInBytes)
{
    verifyFunkinf(SizeInBytes <= m_SizeInBytes, "Attempted to write {} bytes into a uniform buffer only {} bytes large!", SizeInBytes, m_SizeInBytes)
    
    const CVulkanMemoryAllocator& MemoryAllocator = m_VulkanContext.GetMemoryAllocator();
    
    void* DestinationBuffer = MemoryAllocator.MapMemory(m_AllocatedUniformBuffer.BufferAllocation);
    
    memcpy(DestinationBuffer, Data, SizeInBytes);
    
    MemoryAllocator.UnmapMemory(m_AllocatedUniformBuffer.BufferAllocation);
}
