#pragma once

#include "Core/Logging/Logging.h"

#define FUNKIN_DEBUGBREAK() *reinterpret_cast<int32*>(3) = 13

#ifdef FUNKIN_BUILD_DEBUG
    #define FUNKIN_ENABLE_ASSERTIONS
#endif

#define FUNKIN_ENABLE_VERIFIES

/**
 * The difference between the macros suffixed with an 'f' and the ones that aren't is the ones that are suffixed support logging a message,
 * while the ones that aren't suffixed don't support logging a message.
 */
#ifdef FUNKIN_ENABLE_ASSERTIONS
    #define ENGINE_ASSERT_INTERNAL()                     CLogging::LogAssertMessage(ELogType::Engine, "Assertion Failed", "")
    #define GAME_ASSERT_INTERNAL()                       CLogging::LogAssertMessage(ELogType::Game, "Assertion Failed", "")

    #define ENGINE_ASSERT_MESSAGE_INTERNAL(Message, ...) CLogging::LogAssertMessage(ELogType::Engine, "Assertion Failed", Message, ##__VA_ARGS__)
    #define GAME_ASSERT_MESSAGE_INTERNAL(Message, ...)   CLogging::LogAssertMessage(ELogType::Game, "Assertion Failed", Message, ##__VA_ARGS__)

    #define assertEngine(Condition) { if(!(Condition)) { ENGINE_ASSERT_INTERNAL(); FUNKIN_DEBUGBREAK(); } }
    #define assertGame(Condition) { if(!(Condition)) { GAME_ASSERT_INTERNAL(); FUNKIN_DEBUGBREAK(); } }

    #define assertEnginef(Condition, Message, ...) { if(!(Condition)) { ENGINE_ASSERT_MESSAGE_INTERNAL(Message, ##__VA_ARGS__); FUNKIN_DEBUGBREAK(); } }
    #define assertGamef(Condition, Message, ...) { if(!(Condition)) { GAME_ASSERT_MESSAGE_INTERNAL(Message, ##__VA_ARGS__); FUNKIN_DEBUGBREAK(); } }
#else
    // Ensures 'Condition' is still executed in builds where assertions are disabled.
    // This prevents the breaking of code that relies on side effects inside the assertion (e.g., function calls).
    // The (void) cast discards the resulting value, allowing the compiler to optimize out the branch entirely while keeping the expression valid.

    #define assertEngine(Condition) do { (void)(Condition); } while (false);
    #define assertGame(Condition) do { (void)(Condition); } while (false);

    #define assertEnginef(Condition, Message, ...);
    #define assertGamef(Condition, Message, ...);
#endif

#ifdef FUNKIN_ENABLE_VERIFIES
    #define ENGINE_VERIFY_INTERNAL()                     CLogging::LogAssertMessage(ELogType::Engine, "Verify Failed", "")
    #define GAME_VERIFY_INTERNAL()                       CLogging::LogAssertMessage(ELogType::Game, "Verify Failed", "")

    #define ENGINE_VERIFY_MESSAGE_INTERNAL(Message, ...) CLogging::LogAssertMessage(ELogType::Engine, "Verify Failed", Message, ##__VA_ARGS__)
    #define GAME_VERIFY_MESSAGE_INTERNAL(Message, ...)   CLogging::LogAssertMessage(ELogType::Game, "Verify Failed", Message, ##__VA_ARGS__)

    #define verifyEngine(Condition) { if(!(Condition)) { ENGINE_VERIFY_INTERNAL(); FUNKIN_DEBUGBREAK(); } }
    #define verifyGame(Condition) { if(!(Condition)) { GAME_VERIFY_INTERNAL(); FUNKIN_DEBUGBREAK(); } }

    #define verifyEnginef(Condition, Message, ...) { if(!(Condition)) { ENGINE_VERIFY_MESSAGE_INTERNAL(Message, ##__VA_ARGS__); FUNKIN_DEBUGBREAK(); } }
    #define verifyGamef(Condition, Message, ...) { if(!(Condition)) { GAME_VERIFY_MESSAGE_INTERNAL(Message, ##__VA_ARGS__); FUNKIN_DEBUGBREAK(); } }
#else
    // The same principle as above with the assertions.
    // Ensures that 'Condition' is still executed even when verifies are disabled while discarding the resulting value.

    #define verifyEngine(Condition) do { (void)(Condition); } while (false);
    #define verifyGame(Condition) do { (void)(Condition); } while (false);

    #define verifyEnginef(Condition, Message, ...) do { (void)(Condition); } while (false);
    #define verifyGamef(Condition, Message, ...) do { (void)(Condition); } while (false);
#endif