#pragma once

#include "Core/CoreDefines.h"
#include "Generic/GenericApplication.h"

class FUNKIN_API CDesktopApplication : public IGenericApplication
{
public:
    ~CDesktopApplication() override = default;
    
    bool Initialize() override;
    void Shutdown() override;
    
    bool PumpMessages() override;
};
