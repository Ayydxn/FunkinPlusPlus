#include "FunkinPCH.h"
#include "VulkanTexture.h"
#include "VulkanDebugUtils.h"
#include "VulkanUtils.h"
#include "Utils/FileUtils.h"

#include <stb_image.h>

namespace
{
    vk::Format GetVulkanImageFormat(ETextureFormat TextureFormat)
    {
        switch (TextureFormat)
        {
            case ETextureFormat::None: return vk::Format::eUndefined;
            case ETextureFormat::RGBA8: return vk::Format::eR8G8B8A8Unorm;
        }
        
        verifyFunkinf(false, "Failed to get Vulkan image format for unknown texture format!")
        return vk::Format::eUndefined;
    }
}

CVulkanTexture::CVulkanTexture(CVulkanContext& VulkanContext, const FTextureDescription& Description)
    : m_VulkanContext(VulkanContext)
{
    Initialize(Description);
}

CVulkanTexture::CVulkanTexture(CVulkanContext& VulkanContext, const std::filesystem::path& Filepath)
    : m_VulkanContext(VulkanContext)
{
    int32 Width = 0;
    int32 Height = 0;
    
    // Always decode to 4 channels: Vulkan has no reliable support for 3-channel RGB8 sampled images, and a single format keeps every loaded texture interchangeable.
    // The image isn't flipped vertically. Images are stored top row first, which is also where Vulkan's UV origin is (unlike OpenGL's).
    stbi_uc* PixelData;
    {
        // The encoded file only needs to be in memory until it's decoded, so it's scoped to keep it from adding to the peak memory use of big atlases.
        const std::vector<uint8_t> FileContents = CFileUtils::ReadBinaryFile(Filepath);
        verifyFunkinf(!FileContents.empty(), "Failed to read texture file '{}'!", CFileUtils::RedactUserFolderFromFilepath(Filepath))
        
        PixelData = stbi_load_from_memory(FileContents.data(), static_cast<int>(FileContents.size()), &Width, &Height, nullptr, STBI_rgb_alpha);
        verifyFunkinf(PixelData != nullptr, "Failed to decode texture '{}': {}", CFileUtils::RedactUserFolderFromFilepath(Filepath), stbi_failure_reason())
    }
    
    FTextureDescription TextureDescription;
    TextureDescription.Width = static_cast<uint32>(Width);
    TextureDescription.Height = static_cast<uint32>(Height);
    TextureDescription.Format = ETextureFormat::RGBA8;
    
    Initialize(TextureDescription);
    SetData(PixelData, static_cast<uint64>(Width) * static_cast<uint64>(Height) * GetTextureFormatBytesPerPixel(TextureDescription.Format));
    
    stbi_image_free(PixelData);
}

CVulkanTexture::~CVulkanTexture()
{
    const CVulkanMemoryAllocator& MemoryAllocator = m_VulkanContext.GetMemoryAllocator();
    const vk::Device LogicalDevice = m_VulkanContext.GetDevice().GetLogicalDevice();
    
    LogicalDevice.destroyImageView(m_ImageView);
    
    MemoryAllocator.DestroyImage(m_AllocatedImage);
}

void CVulkanTexture::SetData(const void* Data, uint64 SizeInBytes)
{
    const uint64 ExpectedSizeInBytes = static_cast<uint64>(m_Width) * static_cast<uint64>(m_Height) * GetTextureFormatBytesPerPixel(m_Format);
    verifyFunkinf(SizeInBytes == ExpectedSizeInBytes, "Texture data is {} bytes, but a {}x{} texture needs exactly {} bytes!", SizeInBytes, m_Width, m_Height, ExpectedSizeInBytes)
    
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
    
    VulkanDevice.ImmediateSubmit([&StagingBuffer, this](vk::CommandBuffer CommandBuffer)
    {
        CVulkanUtils::TransitionImageLayout(CommandBuffer, m_AllocatedImage.Image, vk::PipelineStageFlagBits2::eTopOfPipe,
            vk::PipelineStageFlagBits2::eTransfer, vk::AccessFlagBits2::eNone, vk::AccessFlagBits2::eTransferWrite,
            vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        
        CVulkanUtils::CopyBufferToImage(CommandBuffer, StagingBuffer.Buffer, m_AllocatedImage.Image, m_Width, m_Height, vk::ImageLayout::eTransferDstOptimal);
        
        CVulkanUtils::TransitionImageLayout(CommandBuffer, m_AllocatedImage.Image, vk::PipelineStageFlagBits2::eTransfer,
            vk::PipelineStageFlagBits2::eFragmentShader, vk::AccessFlagBits2::eTransferWrite, vk::AccessFlagBits2::eShaderRead,
            vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
    });
    
    MemoryAllocator.DestroyBuffer(StagingBuffer);
}

void CVulkanTexture::Initialize(const FTextureDescription& Description)
{
    verifyFunkinf(Description.Format != ETextureFormat::None, "Can't create a texture without a format!")
    verifyFunkinf(Description.Width > 0 && Description.Height > 0, "Can't create a {}x{} texture!", Description.Width, Description.Height)
    
    m_Width = Description.Width;
    m_Height = Description.Height;
    m_Format = Description.Format;
    
    const CVulkanMemoryAllocator& MemoryAllocator = m_VulkanContext.GetMemoryAllocator();
    const vk::Device LogicalDevice = m_VulkanContext.GetDevice().GetLogicalDevice();
    
    vk::ImageCreateInfo ImageCreateInfo = {};
    ImageCreateInfo.sType = vk::StructureType::eImageCreateInfo;
    ImageCreateInfo.imageType = vk::ImageType::e2D;
    ImageCreateInfo.extent = vk::Extent3D(Description.Width, Description.Height, 1);
    ImageCreateInfo.mipLevels = 1;
    ImageCreateInfo.arrayLayers = 1;
    ImageCreateInfo.format = GetVulkanImageFormat(Description.Format);
    ImageCreateInfo.tiling = vk::ImageTiling::eOptimal;
    ImageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
    ImageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
    ImageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
    ImageCreateInfo.samples = vk::SampleCountFlagBits::e1;
    ImageCreateInfo.flags = vk::ImageCreateFlags();
    
    m_AllocatedImage = MemoryAllocator.AllocateImage(ImageCreateInfo, vma::MemoryUsage::eAutoPreferDevice);
    verifyFunkinf(m_AllocatedImage.Image != VK_NULL_HANDLE, "Failed to allocate Vulkan image for texture!")
    
    vk::ImageViewCreateInfo ImageViewCreateInfo = {};
    ImageViewCreateInfo.sType = vk::StructureType::eImageViewCreateInfo;
    ImageViewCreateInfo.image = m_AllocatedImage.Image;
    ImageViewCreateInfo.viewType = vk::ImageViewType::e2D;
    ImageViewCreateInfo.format = GetVulkanImageFormat(Description.Format);
    ImageViewCreateInfo.flags = vk::ImageViewCreateFlags();
    
    ImageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    ImageViewCreateInfo.subresourceRange.levelCount = 1;
    ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    ImageViewCreateInfo.subresourceRange.layerCount = 1;
    
    VK_CHECK_RESULT(LogicalDevice.createImageView(ImageViewCreateInfo, nullptr), m_ImageView, "Failed to create Vulkan image view for texture!")
    
    // A texture can legally be sampled before anything has been uploaded to it, and sampling an image that's still in its undefined layout is invalid. So it starts out
    // cleared to transparent black, in the layout sampling expects (SetData() replaces the contents later). That's one extra blocking submit per texture,
    // which is fine since textures are only created at load boundaries.
    m_VulkanContext.GetDevice().ImmediateSubmit([this](vk::CommandBuffer CommandBuffer)
    {
        CVulkanUtils::TransitionImageLayout(CommandBuffer, m_AllocatedImage.Image, vk::PipelineStageFlagBits2::eTopOfPipe,
            vk::PipelineStageFlagBits2::eTransfer, vk::AccessFlagBits2::eNone, vk::AccessFlagBits2::eTransferWrite,
            vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        
        vk::ImageSubresourceRange ImageSubresourceRange;
        ImageSubresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        ImageSubresourceRange.baseMipLevel = 0;
        ImageSubresourceRange.levelCount = 1;
        ImageSubresourceRange.baseArrayLayer = 0;
        ImageSubresourceRange.layerCount = 1;

        constexpr vk::ClearColorValue TransparentBlack(std::array<float, 4> { 0.0f, 0.0f, 0.0f, 0.0f });
        CommandBuffer.clearColorImage(m_AllocatedImage.Image, vk::ImageLayout::eTransferDstOptimal, TransparentBlack, ImageSubresourceRange);
        
        CVulkanUtils::TransitionImageLayout(CommandBuffer, m_AllocatedImage.Image, vk::PipelineStageFlagBits2::eTransfer,
            vk::PipelineStageFlagBits2::eFragmentShader, vk::AccessFlagBits2::eTransferWrite, vk::AccessFlagBits2::eShaderRead,
            vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
    });
}