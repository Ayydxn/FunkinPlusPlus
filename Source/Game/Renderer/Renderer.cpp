#include "FunkinPCH.h"
#include "Renderer.h"
#include "Misc/Paths.h"

void CRenderer::Initialize()
{
    m_ShaderCompiler.Initialize(m_RHIBackend);
    
    LoadShader(CPaths::GetShadersDirectory() / "TriangleTest");
}

bool CRenderer::BeginFrame(uint32 WindowID) const
{
    return m_DynamicRHI.BeginFrame(WindowID);
}

void CRenderer::EndFrame(uint32 WindowID) const
{
    m_DynamicRHI.EndFrame(WindowID);
}

void CRenderer::BindPipeline(uint32 WindowID, const IGraphicsPipeline& GraphicsPipeline) const
{
    m_DynamicRHI.BindPipeline(WindowID, GraphicsPipeline);
}

void CRenderer::Draw(uint32 WindowID, uint32 VertexCount, uint32 InstanceCount) const
{
    m_DynamicRHI.Draw(WindowID, VertexCount, InstanceCount);
}

std::shared_ptr<IGraphicsPipeline> CRenderer::CreateGraphicsPipeline(const IShader& Shader) const
{
    return ::CreateGraphicsPipeline(m_RHIBackend, m_RHIContext, Shader);
}

void CRenderer::AddShader(const std::shared_ptr<IShader>& Shader)
{
    m_ShaderLibrary.Add(Shader);
}

void CRenderer::LoadShader(const std::filesystem::path& Filepath)
{
    m_ShaderLibrary.Load(Filepath);
}

void CRenderer::LoadShader(const std::string& Name, const std::filesystem::path& Filepath)
{
    m_ShaderLibrary.Load(Name, Filepath);
}
