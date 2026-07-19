#include "FunkinPCH.h"
#include "ShaderCompiler.h"
#include "ShaderCache.h"
#include "Utils/FileUtils.h"

void CShaderCompiler::Initialize(ERHIBackend RHIBackend)
{
    SlangGlobalSessionDesc GlobalSessionDescription = {};
    GlobalSessionDescription.enableGLSL = true;

    const SlangResult GlobalSessionCreationResult = slang::createGlobalSession(&GlobalSessionDescription, m_GlobalSession.writeRef());
    verifyFunkinf(SLANG_SUCCEEDED(GlobalSessionCreationResult), "Failed to create the Slang global session!")
    
    slang::TargetDesc TargetDescription = {};
    TargetDescription.flags = 0;
    
    if (RHIBackend == ERHIBackend::OpenGL)
    {
        TargetDescription.format = SLANG_GLSL;
        TargetDescription.profile = m_GlobalSession->findProfile("glsl_450");
    }
    else if (RHIBackend == ERHIBackend::Vulkan)
    {
        TargetDescription.format = SLANG_SPIRV;
        TargetDescription.profile = m_GlobalSession->findProfile("spirv_1_5");
    }
    else if (RHIBackend == ERHIBackend::Direct3D11)
    {
        TargetDescription.format = SLANG_DXBC;
        TargetDescription.profile = m_GlobalSession->findProfile("sm_5_0");
    }
    else if (RHIBackend == ERHIBackend::Direct3D12)
    {
        TargetDescription.format = SLANG_DXIL;
        TargetDescription.profile = m_GlobalSession->findProfile("sm_6_5");
    }
    else if (RHIBackend == ERHIBackend::Metal)
    {
        TargetDescription.format = SLANG_METAL;
        TargetDescription.profile = m_GlobalSession->findProfile("macos_2_3");
    }

    slang::CompilerOptionEntry CompilerOptionEntry;
    CompilerOptionEntry.name = slang::CompilerOptionName::NoMangle;
    CompilerOptionEntry.value.intValue0 = 1;
    CompilerOptionEntry.value.intValue1 = 1;

    slang::SessionDesc SessionDescription = {};
    SessionDescription.targets = &TargetDescription;
    SessionDescription.targetCount = 1;
    SessionDescription.compilerOptionEntries = &CompilerOptionEntry;
    SessionDescription.compilerOptionEntryCount = 1;

    const SlangResult SessionCreationResult = m_GlobalSession->createSession(SessionDescription, m_Session.writeRef());
    verifyFunkinf(SLANG_SUCCEEDED(SessionCreationResult), "Failed to create the Slang compilation session!")
}

void CShaderCompiler::CompileShader(const std::string& ShaderName, const std::string& ShaderSource, std::unordered_map<SlangStage, FCompiledShaderStage>& CompilationOutput)
{
    LOG_INFO_TAG("Renderer", "Compiling shader '{}'...", ShaderName);

    // Load the shader module
    slang::IModule* SlangModule;
    {
        Slang::ComPtr<slang::IBlob> DiagnosticsBlob;

        SlangModule = m_Session->loadModuleFromSourceString(ShaderName.c_str(), (ShaderName + ".slang").c_str(), ShaderSource.c_str(),
            DiagnosticsBlob.writeRef());

        DiagnoseIfNeeded(DiagnosticsBlob);

        if (!SlangModule)
            return;
    }

    // Query the entry points and compose both them and the module
    const uint32 EntryPointCount = SlangModule->getDefinedEntryPointCount();

    std::vector<slang::IComponentType*> ComponentTypes;
    ComponentTypes.push_back(SlangModule);

    std::vector<Slang::ComPtr<slang::IEntryPoint>> EntryPoints;
    
    for (uint32 i = 0; i < EntryPointCount; i++)
    {
        Slang::ComPtr<slang::IEntryPoint> EntryPoint;

        SlangModule->getDefinedEntryPoint(static_cast<int32>(i), EntryPoint.writeRef());

        EntryPoints.push_back(EntryPoint);
        ComponentTypes.push_back(EntryPoint);
    }

    Slang::ComPtr<slang::IComponentType> ComposedProgram;
    {
        Slang::ComPtr<slang::IBlob> DiagnosticsBlob;

        const SlangResult Result = m_Session->createCompositeComponentType(ComponentTypes.data(), static_cast<int64>(ComponentTypes.size()),
            ComposedProgram.writeRef(), DiagnosticsBlob.writeRef());

        DiagnoseIfNeeded(DiagnosticsBlob);

        if (SLANG_FAILED(Result))
            return;
    }

    // Retrieve and store the SPIR-V bytecode for each entry point
    for (uint32 i = 0; i < EntryPointCount; i++)
    {
        Slang::ComPtr<slang::IBlob> DiagnosticsBlob;

        slang::IEntryPoint* EntryPoint = EntryPoints[i];
        const char* EntryPointName = EntryPoint->getFunctionReflection()->getName();

        Slang::ComPtr<slang::IBlob> BytecodeBlob;
        const SlangResult Result = ComposedProgram->getEntryPointCode(static_cast<int32>(i), 0, BytecodeBlob.writeRef(), DiagnosticsBlob.writeRef());

        DiagnoseIfNeeded(DiagnosticsBlob);

        if (SLANG_FAILED(Result))
            continue;

        if (BytecodeBlob)
        {
            const auto Start = static_cast<const uint32*>(BytecodeBlob->getBufferPointer());
            const size_t WordCount = BytecodeBlob->getBufferSize() / sizeof(uint32);

            FCompiledShaderStage CompiledShaderStage;
            CompiledShaderStage.ShaderStage = EntryPoint->getLayout()->getEntryPointByIndex(0)->getStage();
            CompiledShaderStage.EntryPointName = EntryPointName;
            CompiledShaderStage.Bytecode = std::vector(Start, Start + WordCount);

            CompilationOutput[CompiledShaderStage.ShaderStage] = std::move(CompiledShaderStage);
        }
    }
}

void CShaderCompiler::CompileShaderFromFile(const std::filesystem::path& ShaderFilepath, std::unordered_map<SlangStage, FCompiledShaderStage>& CompilationOutput)
{
    // TODO: (Ayydxn) Gate this behind some sort of renderer setting to toggle shader caching. For now, it's always enabled.
    const std::string ShaderSource = CFileUtils::ReadFile(ShaderFilepath);
    const uint64 SourceHash = CShaderCache::ComputeSourceHash(ShaderSource);
    const std::filesystem::path CacheFilepath = CShaderCache::GetCacheFilepath(ShaderFilepath.stem().string());

    if (CShaderCache::TryLoad(CacheFilepath, SourceHash, CompilationOutput))
        return;

    CompileShader(ShaderFilepath.string(), ShaderSource, CompilationOutput);
    CShaderCache::Write(CacheFilepath, SourceHash, CompilationOutput);
}

void CShaderCompiler::DiagnoseIfNeeded(slang::IBlob* DiagnosticsBlob)
{
    if (DiagnosticsBlob != nullptr)
        LOG_ERROR_TAG("Renderer", "{}", static_cast<const char*>(DiagnosticsBlob->getBufferPointer()));
}
