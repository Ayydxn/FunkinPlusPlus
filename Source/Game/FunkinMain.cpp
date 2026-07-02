#include "FunkinPCH.h"
#include "Engine/FunkinLaunch.h"

#ifdef FUNKIN_PLATFORM_WIN64

#include <Windows.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
    return FunkinMain(__argc, __argv);
}

#else

int main(int ArgumentsCount, char** Arguments)
{
    return FunkinMain(ArgumentsCount, Arguments);
}

#endif
