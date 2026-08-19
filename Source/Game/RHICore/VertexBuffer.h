#pragma once

#include "Core/Misc/AssertionMacros.h"
#include "RHICore/RHIContext.h"

#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

enum class EShaderDataType
{
    Float,
    Float2,
    Float3,
    Float4,

    Matrix3x3,
    Matrix4x4,

    Int,
    Int2,
    Int3,
    Int4,

    Boolean
};

// (Ayydxn) Ported over from Moonlight largely as-is - this is pure vertex-layout description with no
// backend concept in it, so there's no reason to redesign it for Funkin++.
struct FVertexBufferElement
{
    std::string Name;
    EShaderDataType DataType;
    uint32 Size;
    uint32 Offset;
    bool bIsNormalized;

    FVertexBufferElement() = default;
    FVertexBufferElement(std::string Name, const EShaderDataType DataType, bool bIsNormalized = false)
        : Name(std::move(Name)), DataType(DataType), Size(GetShaderDataTypeSize(DataType)), Offset(0), bIsNormalized(bIsNormalized) {}

    uint32 GetComponentCount() const
    {
        switch (DataType)
        {
            case EShaderDataType::Float:     return 1;
            case EShaderDataType::Float2:    return 2;
            case EShaderDataType::Float3:    return 3;
            case EShaderDataType::Float4:    return 4;
            case EShaderDataType::Matrix3x3: return 9;
            case EShaderDataType::Matrix4x4: return 16;
            case EShaderDataType::Int:       return 1;
            case EShaderDataType::Int2:      return 2;
            case EShaderDataType::Int3:      return 3;
            case EShaderDataType::Int4:      return 4;
            case EShaderDataType::Boolean:   return 1;
        }

        verifyFunkinf(false, "Failed to get component count of vertex buffer element!")
        return 0;
    }
private:
    static uint32 GetShaderDataTypeSize(EShaderDataType ShaderDataType)
    {
        switch (ShaderDataType)
        {
            case EShaderDataType::Float:     return 4;
            case EShaderDataType::Float2:    return 8;
            case EShaderDataType::Float3:    return 12;
            case EShaderDataType::Float4:    return 16;
            case EShaderDataType::Matrix3x3: return 4 * 3 * 3;
            case EShaderDataType::Matrix4x4: return 4 * 4 * 4;
            case EShaderDataType::Int:       return 4;
            case EShaderDataType::Int2:      return 8;
            case EShaderDataType::Int3:      return 12;
            case EShaderDataType::Int4:      return 16;
            case EShaderDataType::Boolean:   return 1;
        }

        verifyFunkinf(false, "Failed to get size of shader data type!")
        return 0;
    }
};

class FVertexBufferLayout
{
public:
    FVertexBufferLayout() = default;
    FVertexBufferLayout(const std::initializer_list<FVertexBufferElement>& VertexBufferElements)
        : m_VertexBufferElements(VertexBufferElements)
    {
        CalculateOffsetsAndStride();
    }

    const std::vector<FVertexBufferElement>& GetElements() const { return m_VertexBufferElements; }
    uint32 GetElementCount() const { return static_cast<uint32>(m_VertexBufferElements.size()); }
    uint32 GetStride() const { return m_Stride; }
    
    [[nodiscard]] std::vector<FVertexBufferElement>::iterator begin() { return m_VertexBufferElements.begin(); }
    [[nodiscard]] std::vector<FVertexBufferElement>::iterator end() { return m_VertexBufferElements.end(); }
    [[nodiscard]] std::vector<FVertexBufferElement>::const_iterator begin() const { return m_VertexBufferElements.begin(); }
    [[nodiscard]] std::vector<FVertexBufferElement>::const_iterator end() const { return m_VertexBufferElements.end(); }
private:
    void CalculateOffsetsAndStride()
    {
        uint32 Offset = 0;
        m_Stride = 0;

        for (auto& VertexBufferElement : m_VertexBufferElements)
        {
            VertexBufferElement.Offset = Offset;
            Offset += VertexBufferElement.Size;
            m_Stride += VertexBufferElement.Size;
        }
    }
private:
    std::vector<FVertexBufferElement> m_VertexBufferElements;

    uint32 m_Stride = 0;
};

enum class EVertexBufferUsage
{
    // Device-local. Created via a staged upload, and SetData() re-runs that same staged copy on demand rather than being cheap to call often.
    Static,
    
    // Host-visible/coherent. SetData() maps, calls to memcpy, unmaps directly - no staging, no submit, safe to call every frame.
    Dynamic
};

struct FVertexBufferDescription
{
    EVertexBufferUsage Usage = EVertexBufferUsage::Static;
    FVertexBufferLayout Layout;
    
    uint64 SizeInBytes = 0;
    
    // Optional initial data, uploaded during creation via the same path SetData() uses later.
    // Leave null to create an empty/uninitialized buffer of SizeInBytes (typical for dynamic buffers you intend to write to post-construction).
    const void* InitialData = nullptr;
};

class IVertexBuffer
{
public:
    virtual ~IVertexBuffer() = default;

    IVertexBuffer(const IVertexBuffer&) = delete;
    IVertexBuffer& operator=(const IVertexBuffer&) = delete;
    
    // Data must fit within the buffer's existing size (SizeInBytes at creation) - this writes into the existing allocation, it doesn't resize.
    // If you wanna grow it, create a new buffer instead.
    virtual void SetData(const void* Data, uint64 SizeInBytes) = 0;
    
    uint64 GetSizeInBytes() const { return m_SizeInBytes; }
    EVertexBufferUsage GetUsage() const { return m_Usage; }
    const FVertexBufferLayout& GetLayout() const { return m_Layout; }
protected:
    IVertexBuffer() = default;
protected:
    uint64 m_SizeInBytes = 0;
    EVertexBufferUsage m_Usage = EVertexBufferUsage::Static;
    FVertexBufferLayout m_Layout;
};

std::shared_ptr<IVertexBuffer> CreateVertexBuffer(ERHIBackend RHIBackend, IRHIContext& RHIContext, const FVertexBufferDescription& Description);
