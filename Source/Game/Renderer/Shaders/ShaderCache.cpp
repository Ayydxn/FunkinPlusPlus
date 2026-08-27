#include "FunkinPCH.h"
#include "ShaderCache.h"
#include "Core/Logging/Logging.h"
#include "Core/Misc/Paths.h"
#include "Utils/FileUtils.h"

bool CShaderCache::TryLoad(const std::filesystem::path& CacheFilepath, uint64 SourceHash, std::unordered_map<SlangStage, FCompiledShaderStage>& Output,
    FShaderReflectionData& ReflectionOutput)
{
    std::ifstream FileReader(CacheFilepath, std::ios::in | std::ios::binary);

    if (!FileReader.is_open())
        return false;

    FShaderCacheHeader ShaderCacheHeader;
    FileReader.read(reinterpret_cast<char*>(&ShaderCacheHeader), sizeof(FShaderCacheHeader));

    const std::string RedactedCacheFilepath = CFileUtils::RedactUserFolderFromFilepath(CacheFilepath);

    if (ShaderCacheHeader.MagicNumber != CURRENT_MAGIC_NUMBER_IDENTIFIER || ShaderCacheHeader.Version != CURRENT_SHADER_CACHE_VERSION)
    {
        LOG_WARN_TAG("Renderer", "Shader cache file '{}' is invalid or outdated! Recompiling...", RedactedCacheFilepath);
        return false;
    }

    if (ShaderCacheHeader.SourceHash != SourceHash)
    {
        LOG_INFO_TAG("Renderer", "Shader source has changed for '{}'! Recompiling...", RedactedCacheFilepath);
        return false;
    }

    LOG_INFO_TAG("Renderer", "Loading shader from cache '{}'...", RedactedCacheFilepath);
    
    // The reflection block comes immediately after the header, before any stage/bytecode data.
    for (uint32 i = 0; i < ShaderCacheHeader.ReflectedResourceCount; i++)
    {
        FShaderCacheReflectedResourceHeader CacheReflectedResourceHeader;
        FileReader.read(reinterpret_cast<char*>(&CacheReflectedResourceHeader), sizeof(FShaderCacheReflectedResourceHeader));

        std::string Name(CacheReflectedResourceHeader.NameLength, '\0');
        FileReader.read(Name.data(), CacheReflectedResourceHeader.NameLength);

        FShaderReflectedResource ShaderReflectedResource;
        ShaderReflectedResource.Name = std::move(Name);
        ShaderReflectedResource.Type = CacheReflectedResourceHeader.Type;
        ShaderReflectedResource.Binding = CacheReflectedResourceHeader.Binding;
        ShaderReflectedResource.Set = CacheReflectedResourceHeader.Set;
        ShaderReflectedResource.ArraySize = CacheReflectedResourceHeader.ArraySize;
        ShaderReflectedResource.StageFlags = CacheReflectedResourceHeader.StageFlags;

        ReflectionOutput.Resources.push_back(std::move(ShaderReflectedResource));
    }

    for (uint32 i = 0; i < ShaderCacheHeader.ReflectedPushConstantCount; i++)
    {
        FShaderCacheReflectedPushConstantHeader CacheReflectedPushConstantHeader;
        FileReader.read(reinterpret_cast<char*>(&CacheReflectedPushConstantHeader), sizeof(FShaderCacheReflectedPushConstantHeader));

        std::string Name(CacheReflectedPushConstantHeader.NameLength, '\0');
        FileReader.read(Name.data(), CacheReflectedPushConstantHeader.NameLength);

        FShaderReflectedPushConstant PushConstant;
        PushConstant.Name = std::move(Name);
        PushConstant.Size = CacheReflectedPushConstantHeader.Size;
        PushConstant.Offset = CacheReflectedPushConstantHeader.Offset;
        PushConstant.StageFlags = CacheReflectedPushConstantHeader.StageFlags;

        ReflectionOutput.PushConstants.push_back(std::move(PushConstant));
    }
    
    for (uint32 i = 0; i < ShaderCacheHeader.ShaderStageCount; i++)
    {
        FShaderCacheStageHeader StageHeader;
        FileReader.read(reinterpret_cast<char*>(&StageHeader), sizeof(FShaderCacheStageHeader));

        std::string EntryPointName(StageHeader.EntryPointNameLength, '\0');
        FileReader.read(EntryPointName.data(), StageHeader.EntryPointNameLength);

        FCompiledShaderStage CompiledStage;
        CompiledStage.ShaderStage = StageHeader.Stage;
        CompiledStage.EntryPointName = EntryPointName;
        CompiledStage.Bytecode.resize(StageHeader.BytecodeSize / sizeof(uint32));

        FileReader.read(reinterpret_cast<char*>(CompiledStage.Bytecode.data()), StageHeader.BytecodeSize);

        Output[StageHeader.Stage] = std::move(CompiledStage);
    }
    
    CShaderReflection::LogShaderReflectionData(CacheFilepath.stem().string(), ReflectionOutput);

    return true;
}

void CShaderCache::Write(const std::filesystem::path& CacheFilepath, const uint64 SourceHash, const std::unordered_map<SlangStage, FCompiledShaderStage>& CompiledStages,
    const FShaderReflectionData& ReflectionData)
{
    const std::string RedactedCacheFilepath = CFileUtils::RedactUserFolderFromFilepath(CacheFilepath);
    
    std::ofstream FileWriter(CacheFilepath, std::ios::out | std::ios::binary);
    if (!FileWriter.is_open())
    {
        LOG_ERROR_TAG("Renderer", "Failed to write shader cache file '{}'!", RedactedCacheFilepath);
        return;
    }

    LOG_INFO_TAG("Renderer", "Writing shader to cache '{}'...", RedactedCacheFilepath);

    FShaderCacheHeader ShaderCacheHeader;
    ShaderCacheHeader.ShaderStageCount = static_cast<uint32>(CompiledStages.size());
    ShaderCacheHeader.SourceHash = SourceHash;
    ShaderCacheHeader.ReflectedResourceCount = static_cast<uint32>(ReflectionData.Resources.size());
    ShaderCacheHeader.ReflectedPushConstantCount = static_cast<uint32>(ReflectionData.PushConstants.size());

    FileWriter.write(reinterpret_cast<const char*>(&ShaderCacheHeader), sizeof(FShaderCacheHeader));
    
    for (const auto& [Name, Type, Binding, Set, ArraySize, ShaderStageFlags] : ReflectionData.Resources)
    {
        FShaderCacheReflectedResourceHeader CacheReflectedResourceHeader;
        CacheReflectedResourceHeader.Type = Type;
        CacheReflectedResourceHeader.Binding = Binding;
        CacheReflectedResourceHeader.Set = Set;
        CacheReflectedResourceHeader.ArraySize = ArraySize;
        CacheReflectedResourceHeader.StageFlags = ShaderStageFlags;
        CacheReflectedResourceHeader.NameLength = static_cast<uint32>(Name.size());

        FileWriter.write(reinterpret_cast<const char*>(&CacheReflectedResourceHeader), sizeof(FShaderCacheReflectedResourceHeader));
        FileWriter.write(Name.data(), static_cast<int64>(Name.size()));
    }

    for (const auto& [Name, Size, Offset, ShaderStageFlags] : ReflectionData.PushConstants)
    {
        FShaderCacheReflectedPushConstantHeader CacheReflectedPushConstantHeader;
        CacheReflectedPushConstantHeader.Size = Size;
        CacheReflectedPushConstantHeader.Offset = Offset;
        CacheReflectedPushConstantHeader.StageFlags = ShaderStageFlags;
        CacheReflectedPushConstantHeader.NameLength = static_cast<uint32>(Name.size());

        FileWriter.write(reinterpret_cast<const char*>(&CacheReflectedPushConstantHeader), sizeof(FShaderCacheReflectedPushConstantHeader));
        FileWriter.write(Name.data(), static_cast<int64>(Name.size()));
    }

    for (const auto& [Stage, CompiledStage] : CompiledStages)
    {
        FShaderCacheStageHeader ShaderCacheStageHeader;
        ShaderCacheStageHeader.Stage = Stage;
        ShaderCacheStageHeader.EntryPointNameLength = static_cast<uint32>(CompiledStage.EntryPointName.size());
        ShaderCacheStageHeader.BytecodeSize = static_cast<uint32>(CompiledStage.Bytecode.size() * sizeof(uint32));

        FileWriter.write(reinterpret_cast<const char*>(&ShaderCacheStageHeader), sizeof(FShaderCacheStageHeader));
        FileWriter.write(CompiledStage.EntryPointName.data(), static_cast<int64>(CompiledStage.EntryPointName.size()));
        FileWriter.write(reinterpret_cast<const char*>(CompiledStage.Bytecode.data()), ShaderCacheStageHeader.BytecodeSize);
    }

    FileWriter.flush();
    FileWriter.close();
}

void CShaderCache::DumpBytecode(const std::filesystem::path& CacheFilepath, const std::filesystem::path& OutputDirectory)
{
    const std::string RedactedCacheFilepath = CFileUtils::RedactUserFolderFromFilepath(CacheFilepath);

    std::ifstream FileReader(CacheFilepath, std::ios::in | std::ios::binary);
    if (!FileReader.is_open())
    {
        LOG_ERROR_TAG("Renderer", "Failed to open cache file '{}' for bytecode export!", RedactedCacheFilepath);
        return;
    }

    FShaderCacheHeader ShaderCacheHeader;
    FileReader.read(reinterpret_cast<char*>(&ShaderCacheHeader), sizeof(FShaderCacheHeader));

    if (ShaderCacheHeader.MagicNumber != CURRENT_MAGIC_NUMBER_IDENTIFIER)
    {
        LOG_ERROR_TAG("Renderer", "Shader cache file '{}' is not a valid Funkin++ shader cache file!", RedactedCacheFilepath);
        return;
    }

    // The reflection block sits between the header and the stage/bytecode blocks.
    // We Skip past it the same way TryLoad reads through it since this function only cares about bytecode.
    for (uint32 i = 0; i < ShaderCacheHeader.ReflectedResourceCount; i++)
    {
        FShaderCacheReflectedResourceHeader CacheReflectedResourceHeader;
        FileReader.read(reinterpret_cast<char*>(&CacheReflectedResourceHeader), sizeof(FShaderCacheReflectedResourceHeader));
        FileReader.seekg(CacheReflectedResourceHeader.NameLength, std::ios::cur);
    }

    for (uint32 i = 0; i < ShaderCacheHeader.ReflectedPushConstantCount; i++)
    {
        FShaderCacheReflectedPushConstantHeader CacheReflectedPushConstantHeader;
        FileReader.read(reinterpret_cast<char*>(&CacheReflectedPushConstantHeader), sizeof(FShaderCacheReflectedPushConstantHeader));
        FileReader.seekg(CacheReflectedPushConstantHeader.NameLength, std::ios::cur);
    }
    
    for (uint32 i = 0; i < ShaderCacheHeader.ShaderStageCount; i++)
    {
        FShaderCacheStageHeader ShaderCacheStageHeader;
        FileReader.read(reinterpret_cast<char*>(&ShaderCacheStageHeader), sizeof(FShaderCacheStageHeader));

        std::string EntryPointName(ShaderCacheStageHeader.EntryPointNameLength, '\0');
        FileReader.read(EntryPointName.data(), ShaderCacheStageHeader.EntryPointNameLength);

        std::vector<uint32> Bytecode(ShaderCacheStageHeader.BytecodeSize / sizeof(uint32));
        FileReader.read(reinterpret_cast<char*>(Bytecode.data()), ShaderCacheStageHeader.BytecodeSize);

        const std::filesystem::path OutputFilepath = OutputDirectory / (CacheFilepath.stem().string() + "_" + EntryPointName + ".spv");
        std::ofstream FileWriter(OutputFilepath, std::ios::out | std::ios::binary);

        if (FileWriter.is_open())
        {
            FileWriter.write(reinterpret_cast<const char*>(Bytecode.data()), ShaderCacheStageHeader.BytecodeSize);
            FileWriter.flush();
            FileWriter.close();

            LOG_INFO_TAG("Renderer", "Exported SPIR-V for entry point '{}' to '{}'", EntryPointName, OutputFilepath.string());
        }
    }
}

std::filesystem::path CShaderCache::GetCacheFilepath(const std::string& ShaderName)
{
    return CPaths::GetShaderCacheDirectory() / (ShaderName + ".fpsc");
}

uint64 CShaderCache::ComputeSourceHash(const std::string& ShaderSource)
{
    return std::hash<std::string>{}(ShaderSource);
}
