/*
工具类，因为每次调用都会陷入内核态，十分慢，所以会有一个全局变量。
*/

#ifndef RPC_COMMON_UTILS_H
#define RPC_COMMON_UTILS_H

#include <string_view>
#include <sys/types.h>
#include <string>
#include <unistd.h>
#include "common/log.h"

namespace rpc {
namespace utils {
// 得到进程号
pid_t get_pid();

// 得到线程号x
pid_t get_thread_id();

// 得到当前时间[点]
int64_t get_now_ms();
} // namespace utils
} // namespace rpc
#endif
