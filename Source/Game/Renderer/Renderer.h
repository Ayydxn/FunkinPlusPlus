#pragma once

#include "RHICore/DynamicRHI.h"

class CRenderer
{
public:
    explicit CRenderer(IDynamicRHI& DynamicRHI)
        : m_DynamicRHI(DynamicRHI) {}
    
    CRenderer(const CRenderer&) = delete;
    CRenderer& operator=(const CRenderer&) = delete;
    
    void BeginFrame() const;
    void EndFrame() const;
private:
    IDynamicRHI& m_DynamicRHI;
};
