#pragma once

#include "Core/CoreDefines.h"

class FUNKIN_API IGenericApplication
{
public:
    virtual ~IGenericApplication() = default;
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    
    virtual bool PumpMessages() = 0;
};

// Subclasses of IGenericApplication implement this in a C++ source file and return the appropriate instance.
// This does mean, however, that we cannot compile multiple IGenericApplication implementations at once.
IGenericApplication* CreateApplication();
