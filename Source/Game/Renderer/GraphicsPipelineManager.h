#pragma once

#include "RHICore/GraphicsPipeline.h"
#include "RHICore/RHIContext.h"

#include <unordered_map>

class CGraphicsPipelineManager
{
public:
    CGraphicsPipelineManager(ERHIBackend RHIBackend, IRHIContext& RHIContext)
        : m_RHIBackend(RHIBackend), m_RHIContext(RHIContext) {}
    
    CGraphicsPipelineManager(const CGraphicsPipelineManager&) = delete;
    CGraphicsPipelineManager& operator=(const CGraphicsPipelineManager&) = delete;
    
    const std::shared_ptr<IGraphicsPipeline>& GetOrCreate(const std::string& Name, const FGraphicsPipelineDescription& GraphicsPipelineDescription);
    
    const std::shared_ptr<IGraphicsPipeline>& GetGraphicsPipeline(const std::string& Name);
private:
    // (Ayydxn) Keyed by hash for fast lookup, but the description is stored alongside the pipeline so a hash
    // collision can be caught with a real equality check rather than silently returning the wrong pipeline.
    std::unordered_map<size_t, std::pair<FGraphicsPipelineDescription, std::shared_ptr<IGraphicsPipeline>>> m_Pipelines;
    
    // (Ayydxn) Name -> description hash. Separate from m_Pipelines so a name is just an index into it, not a second owner of the pipeline itself.
    std::unordered_map<std::string, size_t> m_NameToDescriptionHash;
    
    ERHIBackend m_RHIBackend;
    IRHIContext& m_RHIContext;
};