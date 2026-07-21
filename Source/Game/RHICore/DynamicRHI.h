#pragma once

#include "RHIContext.h"
#include "RHICore/GraphicsPipeline.h"

class IDynamicRHI
{
public:
    virtual ~IDynamicRHI() = default;
    
    IDynamicRHI(const IDynamicRHI&) = delete;
    IDynamicRHI& operator=(const IDynamicRHI&) = delete;
    
    virtual bool BeginFrame() = 0;
    virtual void EndFrame() = 0;
    
    virtual void BindPipeline(const IGraphicsPipeline& GraphicsPipeline) = 0;
    virtual void Draw(uint32 VertexCount, uint32 InstanceCount) = 0;
protected:
    IDynamicRHI() = default;
};

std::unique_ptr<IDynamicRHI> CreateDynamicRHI(ERHIBackend RHIBackend, IRHIContext& RHIContext);