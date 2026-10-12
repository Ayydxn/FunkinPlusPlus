#pragma once

#include "Renderer.h"
#include "Renderer2DTypes.h"
#include "RHICore/IndexBuffer.h"
#include "RHICore/UniformBuffer.h"
#include "RHICore/VertexBuffer.h"

#include <glm/glm.hpp>

class CRenderer2D
{
public:
    explicit CRenderer2D(CRenderer& Renderer);
    
    CRenderer2D(const CRenderer2D&) = delete;
    CRenderer2D& operator=(const CRenderer2D&) = delete;
    
    // WorldToClip maps world space to clip space.
    void BeginFrame(const glm::mat4& WorldToClip);
    void EndFrame();
    
    // Draws a colored quad. Transform maps the unit square onto the quad, see QuadCorners. Color is (R, G, B, A) in [0, 1].
    // Quads on a higher layer are drawn on top of quads on a lower one. Within a layer, the quad submitted last is on top.
    void DrawQuad(uint32 Layer, const glm::mat4& Transform, const glm::vec4& Color);
public:
    static constexpr uint32 MaxQuadsPerFrame = 10000;
    static constexpr uint32 LayerCount = 16;
private:
    // The index pattern is the same for every quad, so one buffer covers any frame.
    // Vertices and uniforms change every frame, and the GPU may still be reading the previous frames' copies, so each of those gets one buffer per frame in flight.
    std::shared_ptr<IIndexBuffer> m_QuadIndexBuffer;
    std::vector<std::shared_ptr<IVertexBuffer>> m_QuadVertexBuffers;
    std::vector<std::shared_ptr<IUniformBuffer>> m_GlobalUniformBuffers;
    
    std::array<std::vector<FQuadVertex>, LayerCount> m_LayerVertices;
    std::vector<FQuadVertex> m_SortedVertices;
    
    std::shared_ptr<IGraphicsPipeline> m_QuadPipeline;
    
    CRenderer& m_Renderer;
    
    glm::mat4 m_WorldToClip = glm::mat4(1.0f);
    uint32 m_QuadCount = 0;
};
