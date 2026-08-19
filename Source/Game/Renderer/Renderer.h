#pragma once

#include "GraphicsPipelineManager.h"
#include "RHICore/DynamicRHI.h"
#include "Shaders/ShaderCompiler.h"
#include "Shaders/ShaderLibrary.h"

class CRenderer
{
public:
    explicit CRenderer(IDynamicRHI& DynamicRHI, ERHIBackend RHIBackend, IRHIContext& RHIContext)
        : m_DynamicRHI(DynamicRHI), m_RHIBackend(RHIBackend), m_RHIContext(RHIContext), m_GraphicsPipelineManager(RHIBackend, RHIContext) {}
    
    CRenderer(const CRenderer&) = delete;
    CRenderer& operator=(const CRenderer&) = delete;
    
    void Initialize();
    
    bool BeginFrame() const;
    void EndFrame() const;
    
    void BindPipeline(const IGraphicsPipeline& GraphicsPipeline) const;
    void BindVertexBuffer(const IVertexBuffer& VertexBuffer) const;
    void BindIndexBuffer(const IIndexBuffer& IndexBuffer) const;
    
    void Draw(uint32 VertexCount, uint32 InstanceCount) const;
    void DrawIndexed(uint32 IndexCount, uint32 InstanceCount) const;
    
    void* GetCurrentCommandBuffer() const { return m_DynamicRHI.GetCurrentCommandBuffer(); }
    
    void AddShader(const std::shared_ptr<IShader>& Shader);

    void LoadShader(const std::filesystem::path& Filepath);
    void LoadShader(const std::string& Name, const std::filesystem::path& Filepath);

    const std::shared_ptr<IShader>& GetShader(const std::string& Name) const { return m_ShaderLibrary.GetShader(Name); }
    
    CShaderLibrary& GetShaderLibrary() { return m_ShaderLibrary; }
    const CShaderLibrary& GetShaderLibrary() const { return m_ShaderLibrary; }
    
    CShaderCompiler& GetShaderCompiler() { return m_ShaderCompiler; }
    const CShaderCompiler& GetShaderCompiler() const { return m_ShaderCompiler; }
    
    CGraphicsPipelineManager& GetGraphicsPipelineManager() { return m_GraphicsPipelineManager; }
    const CGraphicsPipelineManager& GetGraphicsPipelineManager() const { return m_GraphicsPipelineManager; }

    ERHIBackend GetRHIBackend() const { return m_RHIBackend; }
    IRHIContext& GetRHIContext() const { return m_RHIContext; }
private:
    IDynamicRHI& m_DynamicRHI;
    ERHIBackend m_RHIBackend;
    IRHIContext& m_RHIContext;
    
    CShaderCompiler m_ShaderCompiler;
    CShaderLibrary m_ShaderLibrary;
    CGraphicsPipelineManager m_GraphicsPipelineManager;
};
