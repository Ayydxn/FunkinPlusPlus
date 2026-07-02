#include "FunkinPCH.h"
#include "FunkinLaunch.h"
#include "FunkinLoop.h"
#include "Misc/CommandLine.h"

int FunkinMain(int32 ArgumentsCount, char** Arguments)
{
    CCommandLine::Initialize(ArgumentsCount, Arguments);

    CFunkinLoop FunkinLoop;
    if (!FunkinLoop.PreInitialize())
        return EXIT_FAILURE;

    if (!FunkinLoop.Initialize())
        return EXIT_FAILURE;

    FunkinLoop.Tick();
    FunkinLoop.Shutdown();

    return 0;
}
