#include "FunkinPCH.h"
#include "VulkanVertexBuffer.h"

CVulkanVertexBuffer::CVulkanVertexBuffer(CVulkanContext& VulkanContext, const FVertexBufferDescription& Description)
    : m_VulkanContext(VulkanContext)
{
    m_SizeInBytes = Description.SizeInBytes;
    m_Usage = Description.Usage;
    m_Layout = Description.Layout;
    
    const CVulkanMemoryAllocator& MemoryAllocator = VulkanContext.GetMemoryAllocator();
    
    vk::BufferCreateInfo BufferCreateInfo = {};
    BufferCreateInfo.sType = vk::StructureType::eBufferCreateInfo;
    BufferCreateInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
    BufferCreateInfo.size = Description.SizeInBytes;
    BufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;
    
    if (Description.Usage == EVertexBufferUsage::Dynamic)
    {
        m_AllocatedVertexBuffer = MemoryAllocator.AllocateBuffer(BufferCreateInfo, vma::MemoryUsage::eAutoPreferHost,
            vma::AllocationCreateFlagBits::eHostAccessSequentialWrite);
    }
    else
    {
        BufferCreateInfo.usage |= vk::BufferUsageFlagBits::eTransferDst;
        
        m_AllocatedVertexBuffer = MemoryAllocator.AllocateBuffer(BufferCreateInfo, vma::MemoryUsage::eAutoPreferDevice);
    }
    
    if (Description.InitialData)
        SetData(Description.InitialData, Description.SizeInBytes);
}

CVulkanVertexBuffer::~CVulkanVertexBuffer()
{
    const CVulkanMemoryAllocator& MemoryAllocator = m_VulkanContext.GetMemoryAllocator();
    
    MemoryAllocator.DestroyBuffer(m_AllocatedVertexBuffer);
}

void CVulkanVertexBuffer::SetData(const void* Data, uint64 SizeInBytes)
{
    verifyFunkinf(SizeInBytes <= m_SizeInBytes, "Attempted to write {} bytes into a vertex buffer only {} bytes large!", SizeInBytes, m_SizeInBytes)
    
    if (m_Usage == EVertexBufferUsage::Dynamic)
    {
        const CVulkanMemoryAllocator& MemoryAllocator = m_VulkanContext.GetMemoryAllocator();
        
        void* DestinationBuffer = MemoryAllocator.MapMemory(m_AllocatedVertexBuffer.BufferAllocation);
        
        memcpy(DestinationBuffer, Data, SizeInBytes);
        
        MemoryAllocator.UnmapMemory(m_AllocatedVertexBuffer.BufferAllocation);
    }
    else
    {
        UploadDataViaStaging(Data, SizeInBytes);
    }
}

void CVulkanVertexBuffer::UploadDataViaStaging(const void* Data, uint64 SizeInBytes) const
{
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
        
        CommandBuffer.copyBuffer(StagingBuffer.Buffer, m_AllocatedVertexBuffer.Buffer, BufferCopy);
    });
    
    MemoryAllocator.DestroyBuffer(StagingBuffer);
}
