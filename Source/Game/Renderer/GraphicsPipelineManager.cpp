#include "FunkinPCH.h"
#include "GraphicsPipelineManager.h"
#include "Logging/Logging.h"

const std::shared_ptr<IGraphicsPipeline>& CGraphicsPipelineManager::GetOrCreate(const std::string& Name, const FGraphicsPipelineDescription& GraphicsPipelineDescription)
{
    const size_t GraphicsPipelineDescriptionHash = std::hash<FGraphicsPipelineDescription>()(GraphicsPipelineDescription);
    
    const auto ExistingPipelineIterator = m_Pipelines.find(GraphicsPipelineDescriptionHash);
    if (ExistingPipelineIterator != m_Pipelines.end())
    {
        // A hash match alone isn't proof of an actual match; two different descriptions could collide onto the same hash.
        // If they don't actually match, we fall through and build a fresh pipeline rather than risk silently returning the wrong one.
        if (ExistingPipelineIterator->second.first == GraphicsPipelineDescription)
            return ExistingPipelineIterator->second.second;
        
        verifyFunkinf(false, "Graphics pipeline description hash collision detected! A cached pipeline with a matching hash but a different description already exists.")
        
        LOG_ERROR_TAG("Renderer", "Graphics pipeline description hash collision detected for hash {}! Overwriting the cache entry with the new pipeline...",
            GraphicsPipelineDescriptionHash);
        
        // The map is keyed by hash, not description, so a colliding description can't coexist
        // alongside the original under this key; insert_or_assign replaces the stale/mismatched entry.
        // This means the original description's pipeline is evicted from the cache on collision - an accepted tradeoff given how rare collisions are expected to be.
        std::shared_ptr<IGraphicsPipeline> NewGraphicsPipeline = CreateGraphicsPipeline(m_RHIBackend, m_RHIContext, GraphicsPipelineDescription);
        const auto InsertedIterator = m_Pipelines.insert_or_assign(GraphicsPipelineDescriptionHash, std::make_pair(GraphicsPipelineDescription,
            std::move(NewGraphicsPipeline))).first;
        
        m_NameToDescriptionHash.insert_or_assign(Name, GraphicsPipelineDescriptionHash);
        
        return InsertedIterator->second.second;
    }
    
    std::shared_ptr<IGraphicsPipeline> NewGraphicsPipeline = CreateGraphicsPipeline(m_RHIBackend, m_RHIContext, GraphicsPipelineDescription);
    const auto [InsertedIterator, bWasInserted] = m_Pipelines.emplace(GraphicsPipelineDescriptionHash, std::make_pair(GraphicsPipelineDescription,
        std::move(NewGraphicsPipeline)));
    
    m_NameToDescriptionHash.insert_or_assign(Name, GraphicsPipelineDescriptionHash);
    
    return InsertedIterator->second.second;
}

const std::shared_ptr<IGraphicsPipeline>& CGraphicsPipelineManager::GetGraphicsPipeline(const std::string& Name)
{
    verifyFunkinf(m_NameToDescriptionHash.contains(Name), "Graphics pipeline '{}' wasn't found in the graphics pipeline manager!", Name)
    
    const size_t DescriptionHash = m_NameToDescriptionHash.at(Name);
    
    // (Ayydxn) This triggering means that somehow our two maps ended up desyncing meaning something inside of this class is bugged. 
    verifyFunkinf(m_Pipelines.contains(DescriptionHash), "Graphics pipeline '{}' resolved to a description hash with no matching pipeline! (Manager state is desynced)",
        Name)
    
    return m_Pipelines.at(DescriptionHash).second;
}
