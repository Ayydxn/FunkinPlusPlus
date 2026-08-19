#pragma once

#include "RHIContext.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "Misc/Hash.h"

struct FGraphicsPipelineDescription
{
    std::shared_ptr<IShader> Shader;
    FVertexBufferLayout VertexBufferLayout;
    
    std::string DebugName = "GraphicsPipeline";
    
    // Pointer identity, not value equality.
    // Vertex buffer layouts are compared by value, meaning that two descriptions with the same shader but different vertex buffer layouts must NOT collide
    // onto the same cached pipeline, since the layout is baked into VkPipeline.
    bool operator==(const FGraphicsPipelineDescription& Other) const
    {
        return Shader == Other.Shader && AreVertexBufferLayoutsEqual(Other.VertexBufferLayout);
    }
private:
    bool AreVertexBufferLayoutsEqual(const FVertexBufferLayout& Other) const
    {
        if (VertexBufferLayout.GetStride() != Other.GetStride() || VertexBufferLayout.GetElementCount() != Other.GetElementCount())
            return false;
        
        for (size_t Index = 0; Index < VertexBufferLayout.GetElementCount(); ++Index)
        {
            const FVertexBufferElement& A = VertexBufferLayout.GetElements()[Index];
            const FVertexBufferElement& B = Other.GetElements()[Index];
            
            if (A.DataType != B.DataType || A.Offset != B.Offset || A.bIsNormalized != B.bIsNormalized)
                return false;
        }
        
        return true;
    }
};

template<>
struct std::hash<FGraphicsPipelineDescription>
{
    size_t operator()(const FGraphicsPipelineDescription& GraphicsPipelineDescription) const noexcept
    {
        size_t Hash = std::hash<std::shared_ptr<IShader>>()(GraphicsPipelineDescription.Shader);
        
        // Folding stride/element-count into the hash is enough for good distribution without hashing every element.
        // AreVertexBufferLayoutsEqual() above still does the real, exact comparison on collision, same "hash for lookup, equality for truth" split the manager already relies on.
        HashCombine(Hash, GraphicsPipelineDescription.VertexBufferLayout.GetStride());
        HashCombine(Hash, GraphicsPipelineDescription.VertexBufferLayout.GetElementCount());
        
        return Hash;
    }
};

class IGraphicsPipeline
{
public:
    virtual ~IGraphicsPipeline() = default;

    IGraphicsPipeline(const IGraphicsPipeline&) = delete;
    IGraphicsPipeline& operator=(const IGraphicsPipeline&) = delete;
    
    virtual void Invalidate() = 0;
protected:
    IGraphicsPipeline() = default;  
};

std::shared_ptr<IGraphicsPipeline> CreateGraphicsPipeline(ERHIBackend RHIBackend, IRHIContext& RHIContext, const FGraphicsPipelineDescription& Description);
