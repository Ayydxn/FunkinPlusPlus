#pragma once

#include "RHICore/RHIContext.h"
#include "RHICore/Shader.h"

struct FGraphicsPipelineDescription
{
    std::shared_ptr<IShader> Shader;
    
    std::string DebugName = "GraphicsPipeline";
    
    // (Ayydxn) Pointer identity, not value equality.
    // This is safe because CShaderLibrary deduplicates shaders by name, so two shared pointers of IShader referring to "the same shader" always point at the same object.
    bool operator==(const FGraphicsPipelineDescription& Other) const
    {
        return Shader == Other.Shader;
    }
};

template<>
struct std::hash<FGraphicsPipelineDescription>
{
    size_t operator()(const FGraphicsPipelineDescription& GraphicsPipelineDescription) const noexcept
    {
        return std::hash<std::shared_ptr<IShader>>()(GraphicsPipelineDescription.Shader);
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
