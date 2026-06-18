#include "Engine/EngineLaunch.h"

#ifdef FUNKIN_PLATFORM_WIN64

#include <Windows.h>

#include <cstdlib>

int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
    return FunkinEngineMain(__argc, __argv);
}

#else

int main(int ArgumentsCount, char** Arguments)
{
    return FunkinEngineMain(ArgumentsCount, Arguments);
}

#endif
