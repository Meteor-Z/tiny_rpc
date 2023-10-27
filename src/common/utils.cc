// #include <bits/types/struct_timeval.h>
#include "common/utils.h"
#include "sys/time.h"
#include <sys/syscall.h>
#include <unistd.h>

namespace rpc {
namespace utils {
static int g_pid = 0;                    // 进程号
static thread_local int g_thread_id = 0; // 每一个线程的id

pid_t get_thread_id() {
    if (g_thread_id != 0)
        return g_thread_id;
    return syscall(SYS_gettid); // 系统调用
}

pid_t get_pid() {
    if (g_pid != 0)
        return g_pid;
    return getpid();
}

// 这里要学一下，草
int64_t get_now_ms() {
    timeval value;
    gettimeofday(&value, nullptr);
    return value.tv_sec * 1000 + value.tv_usec / 1000;
}
} // namespace utils
} // namespace rpc