#include "utils.h"
#include <unistd.h>
#include <sys/syscall.h>

namespace rpc
{
    namespace utils
    {
        static int g_pid = 0;
        static thread_local int g_thread_id = 0;
        pid_t get_thread_id()
        {
            if (g_thread_id != 0) return g_thread_id;
            return syscall(SYS_gettid); // 系统调用
        }
        pid_t get_pid()
        {
            if (g_pid != 0) return g_pid;
            return getpid();
        }
    }
}