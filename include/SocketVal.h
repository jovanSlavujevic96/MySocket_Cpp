#pragma once

#include <cstdint>

#if defined(_MSC_VER)
typedef uint64_t _SocketVal;
#elif defined(__unix)
typedef int32_t _SocketVal;
#define SOCKET_ERROR SO_ERROR
#define INVALID_SOCKET -1
#endif 
