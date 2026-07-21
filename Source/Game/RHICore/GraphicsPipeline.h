#pragma once

#include "RHICore/RHIContext.h"
#include "RHICore/Shader.h"

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

std::shared_ptr<IGraphicsPipeline> CreateGraphicsPipeline(ERHIBackend RHIBackend, IRHIContext& RHIContext, const IShader& Shader);
