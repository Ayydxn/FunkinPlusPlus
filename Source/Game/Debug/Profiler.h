#pragma once

#define FUNKIN_ENABLE_PROFILING !FUNKIN_BUILD_DISTRIBUTION

#if FUNKIN_ENABLE_PROFILING
    #include <tracy/Tracy.hpp>
#endif

#if FUNKIN_ENABLE_PROFILING
    #define FUNKIN_PROFILE_MARK_FRAME FrameMark

    #define FUNKIN_PROFILE_FUNCTION(...) ZoneScoped##__VA_OPT__(N(__VA_ARGS__))
    #define FUNKIN_PROFILE_SCOPE(...) FUNKIN_PROFILE_FUNCTION(__VA_ARGS__)
    #define FUNKIN_PROFILE_SCOPE_DYNAMIC(Name) ZoneScoped; ZoneName(Name, strlen(Name))
    #define FUNKIN_PROFILE_THREAD(...) tracy::SetThreadName(__VA_ARGS__)
#else
    #define FUNKIN_PROFILE_MARK_FRAME

    #define FUNKIN_PROFILE_FUNCTION(...)
    #define FUNKIN_PROFILE_SCOPE(...)
    #define FUNKIN_PROFILE_SCOPE_DYNAMIC(Name)
    #define FUNKIN_PROFILE_THREAD(...)
#endif