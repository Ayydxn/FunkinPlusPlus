#pragma once

#include "RHICore/DynamicRHI.h"
#include "Shaders/ShaderCompiler.h"
#include "Shaders/ShaderLibrary.h"

class CRenderer
{
public:
    explicit CRenderer(IDynamicRHI& DynamicRHI, ERHIBackend RHIBackend, IRHIContext& RHIContext)
        : m_DynamicRHI(DynamicRHI), m_RHIBackend(RHIBackend), m_RHIContext(RHIContext) {}
    
    CRenderer(const CRenderer&) = delete;
    CRenderer& operator=(const CRenderer&) = delete;
    
    void Initialize();
    
    bool BeginFrame() const;
    void EndFrame() const;
    
    void BindPipeline(const IGraphicsPipeline& GraphicsPipeline) const;
    void Draw(uint32 VertexCount, uint32 InstanceCount) const;
    
    std::shared_ptr<IGraphicsPipeline> CreateGraphicsPipeline(const IShader& Shader) const;
    
    void AddShader(const std::shared_ptr<IShader>& Shader);

    void LoadShader(const std::filesystem::path& Filepath);
    void LoadShader(const std::string& Name, const std::filesystem::path& Filepath);

    const std::shared_ptr<IShader>& GetShader(const std::string& Name) const { return m_ShaderLibrary.GetShader(Name); }
    
    CShaderLibrary& GetShaderLibrary() { return m_ShaderLibrary; }
    const CShaderLibrary& GetShaderLibrary() const { return m_ShaderLibrary; }
    
    CShaderCompiler& GetShaderCompiler() { return m_ShaderCompiler; }
    const CShaderCompiler& GetShaderCompiler() const { return m_ShaderCompiler; }

    ERHIBackend GetRHIBackend() const { return m_RHIBackend; }
    IRHIContext& GetRHIContext() const { return m_RHIContext; }
private:
    IDynamicRHI& m_DynamicRHI;
    ERHIBackend m_RHIBackend;
    IRHIContext& m_RHIContext;
    
    CShaderCompiler m_ShaderCompiler;
    CShaderLibrary m_ShaderLibrary;
};
