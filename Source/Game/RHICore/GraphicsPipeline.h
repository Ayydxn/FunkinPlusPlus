#pragma once

#include "RHIContext.h"
#include "RHITypes.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "Misc/Hash.h"

struct FRasterizerState
{
    ECullMode CullMode = ECullMode::Back;
    EFillMode FillMode = EFillMode::Solid;
    EFrontFace FrontFace = EFrontFace::Clockwise;
    
    float LineWidth = 1.0f;
    
    bool operator==(const FRasterizerState& Other) const
    {
        return CullMode == Other.CullMode && FillMode == Other.FillMode && FrontFace == Other.FrontFace && LineWidth == Other.LineWidth;
    }
};

struct FDepthStencilState
{
    ECompareOperation DepthCompareOp = ECompareOperation::Less;
    
    bool bEnableDepthTesting  = true;
    bool bEnableDepthWriting = true;
    bool bEnableStencil = false;

    bool operator==(const FDepthStencilState& Other) const
    {
        return DepthCompareOp == Other.DepthCompareOp && bEnableDepthTesting == Other.bEnableDepthTesting && bEnableDepthWriting == Other.bEnableDepthWriting
            && bEnableStencil == Other.bEnableStencil;
    }
};

struct FBlendState
{
    EBlendFactor SrcColorBlendFactor = EBlendFactor::SrcAlpha;
    EBlendFactor DstColorBlendFactor = EBlendFactor::OneMinusSrcAlpha;
    EBlendOperation ColorBlendOperation = EBlendOperation::Add;
    
    EBlendFactor SrcAlphaBlendFactor = EBlendFactor::One;
    EBlendFactor DstAlphaBlendFactor = EBlendFactor::Zero;
    EBlendOperation AlphaBlendOperation = EBlendOperation::Add;
    
    bool bEnableBlending = false;

    bool operator==(const FBlendState& Other) const
    {
        return SrcColorBlendFactor == Other.SrcColorBlendFactor && DstColorBlendFactor == Other.DstColorBlendFactor &&
               ColorBlendOperation == Other.ColorBlendOperation && SrcAlphaBlendFactor == Other.SrcAlphaBlendFactor && DstAlphaBlendFactor == Other.DstAlphaBlendFactor &&
               AlphaBlendOperation == Other.AlphaBlendOperation && bEnableBlending == Other.bEnableBlending;
    }
};

struct FGraphicsPipelineDescription
{
    std::shared_ptr<IShader> Shader;
    FVertexBufferLayout VertexBufferLayout;
    EPrimitiveTopology PrimitiveTopology = EPrimitiveTopology::Triangles;
    FRasterizerState RasterizerState;
    FDepthStencilState DepthStencilState;
    FBlendState BlendState;
    
    std::string DebugName = "GraphicsPipeline";
    
    // Pointer identity, not value equality.
    // Vertex buffer layouts are compared by value, meaning that two descriptions with the same shader but different vertex buffer layouts must NOT collide
    // onto the same cached pipeline, since the layout is baked into VkPipeline.
    bool operator==(const FGraphicsPipelineDescription& Other) const
    {
        return Shader == Other.Shader && AreVertexBufferLayoutsEqual(Other.VertexBufferLayout) && PrimitiveTopology == Other.PrimitiveTopology
            && RasterizerState == Other.RasterizerState && DepthStencilState == Other.DepthStencilState && BlendState == Other.BlendState;
    }
private:
    bool AreVertexBufferLayoutsEqual(const FVertexBufferLayout& Other) const
    {
        if (VertexBufferLayout.GetStride() != Other.GetStride() || VertexBufferLayout.GetElementCount() != Other.GetElementCount() ||
            VertexBufferLayout.GetInputRate() != Other.GetInputRate())
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
        HashCombine(Hash, GraphicsPipelineDescription.VertexBufferLayout.GetInputRate());
        
        // Primitive Topology
        HashCombine(Hash, static_cast<uint32>(GraphicsPipelineDescription.PrimitiveTopology));
        
        // Rasterizer State
        HashCombine(Hash, static_cast<uint32>(GraphicsPipelineDescription.RasterizerState.CullMode));
        HashCombine(Hash, static_cast<uint32>(GraphicsPipelineDescription.RasterizerState.FillMode));
        HashCombine(Hash, static_cast<uint32>(GraphicsPipelineDescription.RasterizerState.FrontFace));
        HashCombine(Hash, GraphicsPipelineDescription.RasterizerState.LineWidth);
        
        // Depth-Stencil State
        HashCombine(Hash, static_cast<uint32>(GraphicsPipelineDescription.DepthStencilState.DepthCompareOp));
        HashCombine(Hash, GraphicsPipelineDescription.DepthStencilState.bEnableDepthTesting);
        HashCombine(Hash, GraphicsPipelineDescription.DepthStencilState.bEnableDepthWriting);
        HashCombine(Hash, GraphicsPipelineDescription.DepthStencilState.bEnableStencil);
        
        // Blend State
        HashCombine(Hash, GraphicsPipelineDescription.BlendState.bEnableBlending);
        HashCombine(Hash, static_cast<uint32>(GraphicsPipelineDescription.BlendState.SrcColorBlendFactor));
        HashCombine(Hash, static_cast<uint32>(GraphicsPipelineDescription.BlendState.DstColorBlendFactor));
        HashCombine(Hash, static_cast<uint32>(GraphicsPipelineDescription.BlendState.ColorBlendOperation));
        HashCombine(Hash, static_cast<uint32>(GraphicsPipelineDescription.BlendState.SrcAlphaBlendFactor));
        HashCombine(Hash, static_cast<uint32>(GraphicsPipelineDescription.BlendState.DstAlphaBlendFactor));
        HashCombine(Hash, static_cast<uint32>(GraphicsPipelineDescription.BlendState.AlphaBlendOperation));
        
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
