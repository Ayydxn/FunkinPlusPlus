#pragma once

#include <string>

enum class ERHIBackend;
class IRHIContext;
class CShaderCompiler;

class IShader
{
public:
    virtual ~IShader() = default;

    IShader(const IShader&) = delete;
    IShader& operator=(const IShader&) = delete;

    const std::string& GetName() const { return m_Name; }
protected:
    IShader() = default;
protected:
    std::string m_Name;
};

std::shared_ptr<IShader> CreateShader(ERHIBackend RHIBackend, IRHIContext& RHIContext, CShaderCompiler& ShaderCompiler, const std::string& Name, const std::string& Source);
std::shared_ptr<IShader> CreateShader(ERHIBackend RHIBackend, IRHIContext& RHIContext, CShaderCompiler& ShaderCompiler, const std::filesystem::path& Filepath);