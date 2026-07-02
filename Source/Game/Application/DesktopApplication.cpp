#include "FunkinPCH.h"
#include "DesktopApplication.h"

bool CDesktopApplication::Initialize(CEngineContext& EngineContext)
{
    m_EngineContext = &EngineContext;
    
    return true;
}

void CDesktopApplication::Shutdown()
{
}

void CDesktopApplication::PumpMessages()
{
}
