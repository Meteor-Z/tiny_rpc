#ifndef RPC_COMMON_UTILS_H
#define RPC_COMMON_UTILS_H

#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <string_view>
#include "log.h"

namespace rpc
{
    namespace utils
    {
        pid_t get_pid();
        pid_t get_thread_id();
        int64_t get_now_ms();
    }
}
#endif
