#include "FunkinPCH.h"
#include "EngineLaunch.h"
#include "EngineLoop.h"

int FunkinEngineMain(int ArgumentsCount, char** Arguments)
{
    const auto Application = CreateApplication();
    
    CFunkinEngineLoop FunkinEngineLoop(Application);
        
    if (!FunkinEngineLoop.PreInitialize())
        return -1;
        
    if (!FunkinEngineLoop.Initialize())
        return -1;
        
    FunkinEngineLoop.Tick();
    FunkinEngineLoop.Shutdown();
    
    delete Application;
        
    return 0;
}
