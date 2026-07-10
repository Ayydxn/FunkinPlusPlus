#pragma once
#include "RHICore/RHIContext.h"

class CRenderer
{
public:
    explicit CRenderer(IRHIContext& RHIContext)
        : m_RHIContext(RHIContext) {}
    
    CRenderer(const CRenderer&) = delete;
    CRenderer& operator=(const CRenderer&) = delete;
private:
    IRHIContext& m_RHIContext;
};
