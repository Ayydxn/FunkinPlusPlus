#pragma once

#include "RHICore/RHIContext.h"

#include <slang-com-ptr.h>
#include <slang.h>

struct FCompiledShaderStage
{
    SlangStage ShaderStage = SLANG_STAGE_NONE;
    std::string EntryPointName;
    std::vector<uint32> Bytecode;
};

class CShaderCompiler
{
public:
    CShaderCompiler() = default;
    ~CShaderCompiler() = default;

    CShaderCompiler(const CShaderCompiler&) = delete;
    CShaderCompiler& operator=(const CShaderCompiler&) = delete;

    void Initialize(ERHIBackend RHIBackend);

    void CompileShader(const std::string& ShaderName, const std::string& ShaderSource, std::unordered_map<SlangStage, FCompiledShaderStage>& CompilationOutput);
    void CompileShaderFromFile(const std::filesystem::path& ShaderFilepath, std::unordered_map<SlangStage, FCompiledShaderStage>& CompilationOutput);
private:
    void DiagnoseIfNeeded(slang::IBlob* DiagnosticsBlob);
private:
    Slang::ComPtr<slang::IGlobalSession> m_GlobalSession;
    Slang::ComPtr<slang::ISession> m_Session;
};
