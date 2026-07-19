#include "FunkinPCH.h"
#include "ShaderLibrary.h"
#include "Core/Logging/Logging.h"
#include "Engine/EngineContext.h"

void CShaderLibrary::Add(const std::shared_ptr<IShader>& Shader)
{
    const std::string& Name = Shader->GetName();
    if (m_Shaders.contains(Name))
    {
        LOG_WARN_TAG("Renderer", "Shader '{}' is already present in the shader library", Name);
        return;
    }

    m_Shaders[Name] = Shader;
}

void CShaderLibrary::Load(const std::filesystem::path& Filepath)
{
    CRenderer& Renderer = CEngineContext::GetInstance().GetRenderer();
    const std::filesystem::path FullFilepath = Filepath.string() + ".slang";
    const std::shared_ptr<IShader> Shader = CreateShader(Renderer.GetRHIBackend(), Renderer.GetRHIContext(), Renderer.GetShaderCompiler(), FullFilepath);
    
    Add(Shader);
}

void CShaderLibrary::Load(const std::string& Name, const std::filesystem::path& Filepath)
{
    CRenderer& Renderer = CEngineContext::GetInstance().GetRenderer();
    const std::filesystem::path FullFilepath = Filepath.string() + ".slang";
    const std::shared_ptr<IShader> Shader = CreateShader(Renderer.GetRHIBackend(), Renderer.GetRHIContext(), Renderer.GetShaderCompiler(), FullFilepath);
    
    Add(Shader);
}

const std::shared_ptr<IShader>& CShaderLibrary::GetShader(const std::string& Name) const
{
    verifyFunkinf(m_Shaders.contains(Name), "Shader '{}' wasn't found in the shader library!", Name)

    return m_Shaders.at(Name);
}