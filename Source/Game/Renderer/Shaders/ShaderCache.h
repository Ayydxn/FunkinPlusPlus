#pragma once

#include "ShaderCompiler.h"

#include <slang.h>

#include <filesystem>
#include <unordered_map>

#define CURRENT_MAGIC_NUMBER_IDENTIFIER 0x66707363 // "fpsc" - Funkin++ Shader Cache
#define CURRENT_SHADER_CACHE_VERSION 1

struct FShaderCacheHeader
{
    // A fixed identifier that says "This is a Funkin++ shader cache file". Should never be changed.
    uint32 MagicNumber = CURRENT_MAGIC_NUMBER_IDENTIFIER;

    // Only bump this when a breaking change is made to either this struct or FShaderCacheStageHeader.
    // Breaking changes can be considered as things such as
    // - Adding/removing fields
    // - Changing a field's type
    // - The ordering of fields
    // - How shader stage data is laid out in the cache file.
    // When bumping, change the number represented by the macro. Not the value of this.
    uint32 Version = CURRENT_SHADER_CACHE_VERSION;

    uint32 ShaderStageCount = 0;
    uint64 SourceHash = 0;
};

struct FShaderCacheStageHeader
{
    SlangStage Stage = SLANG_STAGE_NONE;
    uint32 EntryPointNameLength = 0;
    uint32 BytecodeSize = 0;
};

class CShaderCache
{
public:
    static bool TryLoad(const std::filesystem::path& CacheFilepath, uint64 SourceHash, std::unordered_map<SlangStage, FCompiledShaderStage>& Output);
    static void Write(const std::filesystem::path& CacheFilepath, uint64 SourceHash, const std::unordered_map<SlangStage, FCompiledShaderStage>& CompiledStages);
    static void DumpBytecode(const std::filesystem::path& CacheFilepath, const std::filesystem::path& OutputDirectory);

    static std::filesystem::path GetCacheFilepath(const std::string& ShaderName);
    static uint64 ComputeSourceHash(const std::string& ShaderSource);
};