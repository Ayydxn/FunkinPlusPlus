#include "FunkinPCH.h"
#include "Renderer.h"
#include "Misc/Paths.h"

void CRenderer::Initialize()
{
    m_ShaderCompiler.Initialize(m_RHIBackend);
    
    LoadShader(CPaths::GetShadersDirectory() / "TriangleTest");
}

void CRenderer::BeginFrame() const
{
    m_DynamicRHI.BeginFrame();
}

void CRenderer::EndFrame() const
{
    m_DynamicRHI.EndFrame();
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
