#include "FunkinPCH.h"
#include "EngineLaunch.h"
#include "EngineLoop.h"
#include "Core/CoreTypes.h"
#include "Core/Misc/CommandLine.h"

int FunkinEngineMain(int32 ArgumentsCount, char** Arguments)
{
    const auto Application = CreateApplication();
    
    CCommandLine::Initialize(ArgumentsCount, Arguments);
    
    CFunkinEngineLoop FunkinEngineLoop(Application);
    if (!FunkinEngineLoop.PreInitialize())
        return EXIT_FAILURE;
        
    if (!FunkinEngineLoop.Initialize())
        return EXIT_FAILURE;
        
    FunkinEngineLoop.Tick();
    FunkinEngineLoop.Shutdown();
    
    delete Application;
        
    return 0;
}
