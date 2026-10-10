#pragma once

#include "VulkanContext.h"
#include "VulkanMemoryAllocator.h"
#include "RHICore/Texture.h"

class CVulkanTexture final : public ITexture
{
public:
    CVulkanTexture(CVulkanContext& VulkanContext, const FTextureDescription& Description);
    CVulkanTexture(CVulkanContext& VulkanContext, const std::filesystem::path& Filepath);
    ~CVulkanTexture() override;
    
    void SetData(const void* Data, uint64 SizeInBytes) override;
    
    vk::ImageView GetImageView() const { return m_ImageView; }
private:
    void Initialize(const FTextureDescription& Description);
private:
    CVulkanContext& m_VulkanContext;
    
    FAllocatedVulkanImage m_AllocatedImage;
    vk::ImageView m_ImageView;
};

