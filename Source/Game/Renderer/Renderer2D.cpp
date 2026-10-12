#include "FunkinPCH.h"
#include "Renderer2D.h"
#include "Renderer2DTypes.h"

CRenderer2D::CRenderer2D(CRenderer& Renderer)
    : m_Renderer(Renderer)
{
    // Every quad is two triangles over its four corners: (0, 1, 2) and (2, 3, 0), offset by four vertices per quad.
    std::vector<uint32> QuadIndices;
    QuadIndices.reserve(static_cast<size_t>(MaxQuadsPerFrame) * 6);
    
    for (uint32 QuadIndex = 0; QuadIndex < MaxQuadsPerFrame; ++QuadIndex)
    {
        const uint32 FirstVertex = QuadIndex * 4;
        
        QuadIndices.insert(QuadIndices.end(), { FirstVertex + 0, FirstVertex + 1, FirstVertex + 2, FirstVertex + 2, FirstVertex + 3, FirstVertex + 0 });
    }
    
    m_QuadIndexBuffer = CreateIndexBuffer(m_Renderer.GetRHIBackend(), m_Renderer.GetRHIContext(), {
        .SizeInBytes = QuadIndices.size() * sizeof(uint32),
        .InitialData = QuadIndices.data()
    });
    
    for (uint32 FrameIndex = 0; FrameIndex < m_Renderer.GetFramesInFlight(); ++FrameIndex)
    {
        m_QuadVertexBuffers.push_back(CreateVertexBuffer(m_Renderer.GetRHIBackend(), m_Renderer.GetRHIContext(), {
            .Usage = EVertexBufferUsage::Dynamic,
            .Layout = GetQuadVertexBufferLayout(),
            .SizeInBytes = static_cast<uint64>(MaxQuadsPerFrame) * 4 * sizeof(FQuadVertex)
        }));
        
        m_GlobalUniformBuffers.push_back(CreateUniformBuffer(m_Renderer.GetRHIBackend(), m_Renderer.GetRHIContext(), {
            .SizeInBytes = sizeof(FGlobalUniforms)
        }));
    }
    
    m_SortedVertices.reserve(static_cast<size_t>(MaxQuadsPerFrame) * 4);
}

void CRenderer2D::BeginFrame(const glm::mat4& WorldToClip)
{
    m_WorldToClip = WorldToClip;
    m_QuadCount = 0;
    
    for (std::vector<FQuadVertex>& LayerVertices : m_LayerVertices)
        LayerVertices.clear();
}

void CRenderer2D::DrawQuad(uint32 Layer, const glm::mat4& Transform, const glm::vec4& Color)
{
    verifyFunkinf(Layer < LayerCount, "Failed to draw a quad! Layer {} is out of range (there are {} layers)!", Layer, LayerCount)
    if (Layer >= LayerCount)
        return;
    
    verifyFunkinf(m_QuadCount < MaxQuadsPerFrame, "Failed to draw a quad! More than {} quads were submitted in a single frame!", MaxQuadsPerFrame)
    if (m_QuadCount >= MaxQuadsPerFrame)
        return;
    
    std::vector<FQuadVertex>& LayerVertices = m_LayerVertices[Layer];
    
    for (const glm::vec2& Corner : QuadCorners)
    {
        FQuadVertex QuadVertex;
        QuadVertex.Position = glm::vec3(Transform * glm::vec4(Corner, 0.0f, 1.0f));
        QuadVertex.Color = Color;
        
        LayerVertices.push_back(QuadVertex);
    }
    
    m_QuadCount++;
}

void CRenderer2D::EndFrame()
{
    if (m_QuadCount == 0)
        return;
    
    // Lower layers first. Each layer already holds its quads in submission order.
    m_SortedVertices.clear();
    
    for (const std::vector<FQuadVertex>& LayerVertices : m_LayerVertices)
        m_SortedVertices.insert(m_SortedVertices.end(), LayerVertices.begin(), LayerVertices.end());
    
    const uint32 FrameIndex = m_Renderer.GetCurrentFrameIndex();
    
    const std::shared_ptr<IVertexBuffer>& QuadVertexBuffer = m_QuadVertexBuffers[FrameIndex];
    QuadVertexBuffer->SetData(m_SortedVertices.data(), m_SortedVertices.size() * sizeof(FQuadVertex));
    
    const FGlobalUniforms GlobalUniforms = {
        .ViewProjectionMatrix = m_WorldToClip
    };
    
    const std::shared_ptr<IUniformBuffer>& GlobalUniformBuffer = m_GlobalUniformBuffers[FrameIndex];
    GlobalUniformBuffer->SetData(&GlobalUniforms, sizeof(FGlobalUniforms));
    
    m_Renderer.BindPipeline(*m_Renderer.GetGraphicsPipelineManager().GetGraphicsPipeline("Renderer2DQuad"));
    m_Renderer.BindUniformBuffer(0, 0, *GlobalUniformBuffer);
    m_Renderer.BindVertexBuffer(*QuadVertexBuffer);
    m_Renderer.BindIndexBuffer(*m_QuadIndexBuffer);
    m_Renderer.DrawIndexed(m_QuadCount * 6, 1);
}
