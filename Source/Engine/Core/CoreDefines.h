#pragma once

#ifdef FUNKIN_EXPORT_LIBRARY
    #ifdef FUNKIN_PLATFORM_WIN64
        #define FUNKIN_API __declspec(dllexport)
    #else
        #define FUNKIN_API __attribute__((visibility("default")))
    #endif
#else
    #ifdef FUNKIN_PLATFORM_WIN64
        #define FUNKIN_API __declspec(dllimport)
    #else
        #define FUNKIN_API
    #endif
#endif
