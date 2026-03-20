#pragma once


// data types
#include <stdint.h>
#include <string>
#include <print>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

// SI conformant memory units
#define KiB(x) x * 1024
#define MiB(x) x * 1024 * 1024
#define GiB(x) x * 1024 * 1024 * 1024


// debugging
#if !NDEBUG
    #if defined(_WIN64) || defined(_WIN32)
        #define DEBUG_BREAK() __debugbreak()
    #elif defined(__linux) || defined(__linux__)
        #include <signal.h>
        #define DEBUG_BREAK() raise(SIGTRAP)
    #else
        #error "Platform currently not supported"
    #endif // platform
#else
    #define DEBUG_BREAK()
#endif // NDEBUG

inline void expect(bool expression, std::string message = "") {
#if !NDEBUG
    if (!expression) {
        if (!message.empty()) {
            std::println(stderr, "Unexcepted error: '{}'", message);
        }
        DEBUG_BREAK();
    }
#endif
}

// Misc.
#define BIT(x) (1 << x)

