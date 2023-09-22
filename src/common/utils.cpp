#include "common/utils.hpp"
#include <bits/types/struct_timeval.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "sys/time.h"

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
        // 这里要学一下，草
        int64_t get_now_ms()
        {
            timeval value;
            gettimeofday(&value, nullptr);
            return value.tv_sec * 1000 + value.tv_usec / 1000;
        }
    }
}