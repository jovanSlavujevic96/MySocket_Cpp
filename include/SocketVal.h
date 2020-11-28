// SocketVal.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <cstdint>

#if defined(_MSC_VER)
typedef uint64_t _SocketVal;
#elif defined(__unix)
typedef int32_t _SocketVal;
#endif 

// TODO: Reference additional headers your program requires here.
