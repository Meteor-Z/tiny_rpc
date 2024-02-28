/*
工具类，因为每次调用都会陷入内核态，十分慢，所以会有一个全局变量。
*/

#ifndef RPC_COMMON_UTILS_H
#define RPC_COMMON_UTILS_H

#include <sys/types.h>
#include <unistd.h>

namespace rpc {
namespace utils {
// 得到进程号
pid_t get_pid();

// 得到线程号x
pid_t get_thread_id();

// 得到当前时间[点]
int64_t get_now_ms();

// 从网络字节序里面传入一个int32的数字
int32_t get_int32_from_netbyte(const char* buf);
} // namespace utils
} // namespace rpc
#endif
