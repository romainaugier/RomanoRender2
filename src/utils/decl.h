#pragma once

// Few declarations utils

#ifdef _MSC_VER

#define FORCEINLINE __forceinline

#elif __GNUC__

#define FORCEINLINE __attribute__((always_inline))

#endif