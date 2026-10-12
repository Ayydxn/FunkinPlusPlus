#pragma once

#include "Renderer2D.h"

class CSceneRenderer
{
public:
    explicit CSceneRenderer(CRenderer& Renderer);
    
    CSceneRenderer(const CSceneRenderer&) = delete;
    CSceneRenderer& operator=(const CSceneRenderer&) = delete;
    
    void Render();
private:
    CRenderer2D m_Renderer2D;
};
