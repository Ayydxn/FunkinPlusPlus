#include "FunkinPCH.h"
#include "VulkanIndexBuffer.h"

CVulkanIndexBuffer::CVulkanIndexBuffer(CVulkanContext& VulkanContext, const FIndexBufferDescription& Description)
    : m_VulkanContext(VulkanContext), m_Size(static_cast<uint32>(Description.SizeInBytes))
{
    const CVulkanMemoryAllocator& MemoryAllocator = VulkanContext.GetMemoryAllocator();
    
    vk::BufferCreateInfo BufferCreateInfo = {};
    BufferCreateInfo.sType = vk::StructureType::eBufferCreateInfo;
    BufferCreateInfo.size = Description.SizeInBytes;
    BufferCreateInfo.usage = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst;
    BufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;
    
    m_AllocatedIndexBuffer = MemoryAllocator.AllocateBuffer(BufferCreateInfo, vma::MemoryUsage::eAutoPreferDevice);
    
    if (Description.InitialData)
        SetData(Description.InitialData, Description.SizeInBytes);
}

CVulkanIndexBuffer::~CVulkanIndexBuffer()
{
    const CVulkanMemoryAllocator& MemoryAllocator = m_VulkanContext.GetMemoryAllocator();
    
    MemoryAllocator.DestroyBuffer(m_AllocatedIndexBuffer);
}

void CVulkanIndexBuffer::SetData(const void* Data, uint64 SizeInBytes)
{
    verifyFunkinf(SizeInBytes <= m_Size, "Attempted to write {} bytes into an index buffer only {} bytes large!", SizeInBytes, m_Size)
    
    const CVulkanMemoryAllocator& MemoryAllocator = m_VulkanContext.GetMemoryAllocator();
    const CVulkanDevice& VulkanDevice = m_VulkanContext.GetDevice();
    
    vk::BufferCreateInfo StagingBufferCreateInfo = {};
    StagingBufferCreateInfo.sType = vk::StructureType::eBufferCreateInfo;
    StagingBufferCreateInfo.size = SizeInBytes;
    StagingBufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
    StagingBufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;
    
    const FAllocatedVulkanBuffer StagingBuffer = MemoryAllocator.AllocateBuffer(StagingBufferCreateInfo, vma::MemoryUsage::eAutoPreferHost,
        vma::AllocationCreateFlagBits::eHostAccessSequentialWrite);
    
    void* DestinationBuffer = MemoryAllocator.MapMemory(StagingBuffer.BufferAllocation);
    
    memcpy(DestinationBuffer, Data, SizeInBytes);
    
    MemoryAllocator.UnmapMemory(StagingBuffer.BufferAllocation);
    
    VulkanDevice.ImmediateSubmit([&SizeInBytes, &StagingBuffer, this](vk::CommandBuffer CommandBuffer)
    {
        vk::BufferCopy BufferCopy = {};
        BufferCopy.size = SizeInBytes;
        
        CommandBuffer.copyBuffer(StagingBuffer.Buffer, m_AllocatedIndexBuffer.Buffer, BufferCopy);
    });
    
    MemoryAllocator.DestroyBuffer(StagingBuffer);
}
