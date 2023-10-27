/*
工具类，因为每次调用都会陷入内核态，十分慢，所以会有一个全局变量。
*/

#ifndef RPC_COMMON_UTILS_H
#define RPC_COMMON_UTILS_H

#include <string_view>
#include <sys/types.h>
#include <string>
#include "log.h"
#include <unistd.h>

namespace rpc {
namespace utils {
pid_t get_pid();
pid_t get_thread_id();
int64_t get_now_ms();
} // namespace utils
} // namespace rpc
#endif
