#ifndef RPC_COMMON_UTILS_H
#define RPC_COMMON_UTILS_H

#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <string_view>
#include "log.hpp"

namespace rpc
{
    namespace utils
    {
        pid_t get_pid();
        pid_t get_thread_id();
    }
}
#endif

