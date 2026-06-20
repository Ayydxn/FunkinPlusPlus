#include "FunkinPCH.h"
#include "EngineLaunch.h"
#include "EngineLoop.h"

int FunkinEngineMain(int ArgumentsCount, char** Arguments)
{
    const auto Application = CreateApplication();
    
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
