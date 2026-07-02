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
    #define ASSERT_INTERNAL()                     CLogging::LogAssertMessage("Assertion Failed", "")
    #define ASSERT_MESSAGE_INTERNAL(Message, ...) CLogging::LogAssertMessage("Assertion Failed", Message, ##__VA_ARGS__)

    #define assertFunkin(Condition) { if(!(Condition)) { ASSERT_INTERNAL(); FUNKIN_DEBUGBREAK(); } }
    #define assertFunkinf(Condition, Message, ...) { if(!(Condition)) { ASSERT_MESSAGE_INTERNAL(Message, ##__VA_ARGS__); FUNKIN_DEBUGBREAK(); } }
#else
    // Ensures 'Condition' is still executed in builds where assertions are disabled.
    // This prevents the breaking of code that relies on side effects inside the assertion (e.g., function calls).
    // The (void) cast discards the resulting value, allowing the compiler to optimize out the branch entirely while keeping the expression valid.

    #define assertFunkin(Condition) do { (void)(Condition); } while (false);
    #define assertFunkinf(Condition, Message, ...) do { (void)(Condition); } while (false);
#endif

#ifdef FUNKIN_ENABLE_VERIFIES
    #define VERIFY_INTERNAL()                     CLogging::LogAssertMessage("Verify Failed", "")
    #define VERIFY_MESSAGE_INTERNAL(Message, ...) CLogging::LogAssertMessage("Verify Failed", Message, ##__VA_ARGS__)

    #define verifyFunkin(Condition) { if(!(Condition)) { VERIFY_INTERNAL(); FUNKIN_DEBUGBREAK(); } }
    #define verifyFunkinf(Condition, Message, ...) { if(!(Condition)) { VERIFY_MESSAGE_INTERNAL(Message, ##__VA_ARGS__); FUNKIN_DEBUGBREAK(); } }
#else
    // The same principle as above with the assertions.
    // Ensures that 'Condition' is still executed even when verifies are disabled while discarding the resulting value.

    #define verifyFunkin(Condition) do { (void)(Condition); } while (false);
    #define verifyFunkinf(Condition, Message, ...) do { (void)(Condition); } while (false);
#endif