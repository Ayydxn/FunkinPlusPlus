#pragma once

#if defined(FUNKIN_PLATFORM_WIN64) || defined(FUNKIN_PLATFORM_MAC) || defined(FUNKIN_PLATFORM_LINUX)
    #include "DesktopApplication.h"
    
    using FPlatformApplication = CDesktopApplication;
#else
    #error "No application implementation exists for this platform!"
#endif