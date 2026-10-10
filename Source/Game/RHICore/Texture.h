#pragma once

#include "RHICore/RHIContext.h"

#include <filesystem>

enum class ETextureFormat : uint8
{
    None = 0,
    
    RGBA8
};

struct FTextureDescription
{
    uint32 Width = 1;
    uint32 Height = 1;
    ETextureFormat Format = ETextureFormat::RGBA8;
};

class ITexture
{
public:
    virtual ~ITexture() = default;
    
    ITexture(const ITexture&) = delete;
    ITexture& operator=(const ITexture&) = delete;
    
    virtual void SetData(const void* Data, uint64 SizeInBytes) = 0;
    
    uint32 GetWidth() const { return m_Width; }
    uint32 GetHeight() const { return m_Height; }
    ETextureFormat GetFormat() const { return m_Format; }
protected:
    ITexture() = default;
protected:
    uint32 m_Width = 0;
    uint32 m_Height = 0;
    ETextureFormat m_Format = ETextureFormat::None;
};

std::shared_ptr<ITexture> CreateTexture(ERHIBackend RHIBackend, IRHIContext& RHIContext, const FTextureDescription& Description);
std::shared_ptr<ITexture> CreateTexture(ERHIBackend RHIBackend, IRHIContext& RHIContext, const std::filesystem::path& Filepath);

uint32 GetTextureFormatBytesPerPixel(ETextureFormat Format);
