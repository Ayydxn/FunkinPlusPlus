#include "FunkinPCH.h"
#include "Renderer.h"

void CRenderer::BeginFrame() const
{
    m_DynamicRHI.BeginFrame();
}

void CRenderer::EndFrame() const
{
    m_DynamicRHI.EndFrame();
}
