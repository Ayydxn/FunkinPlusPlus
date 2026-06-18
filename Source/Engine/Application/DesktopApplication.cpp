#include "FunkinPCH.h"
#include "DesktopApplication.h"

bool CDesktopApplication::Initialize()
{
    printf("Hello World!\n");
    
    return true;
}

void CDesktopApplication::Shutdown()
{
}

bool CDesktopApplication::PumpMessages()
{
    return true;
}

IGenericApplication* CreateApplication()
{
    return new CDesktopApplication();
}