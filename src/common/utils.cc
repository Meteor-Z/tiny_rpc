#include "sys/time.h"
#include <sys/syscall.h>
#include <thread>
#include <unistd.h>
#include "common/utils.h"

namespace rpc {
namespace utils {
static int g_pid = 0;                    // 进程号
static thread_local int g_thread_id = 0; // 线程号，所以用 thread_local

pid_t get_thread_id() {
    if (g_thread_id == 0) {
        g_thread_id = syscall(SYS_gettid);
    }
    return g_thread_id;
}

pid_t get_pid() {
    if (g_pid == 0) {
        g_pid = getpid();
    }
    return g_pid;
}

// struct timeval {
//     long tv_sec;  // 秒
//     long tv_usec; // 微秒
// };

int64_t get_now_ms() {
    timeval value;
    gettimeofday(&value, nullptr);
    // 秒 * 1000 + 微秒 / 1000 = 毫秒
    return value.tv_sec * 1000 + value.tv_usec / 1000;
}
} // namespace utils
} // namespace rpc