#ifndef TINY_RPC_LOG_H
#define TINY_RPC_LOG_H

#include <string>
#include <fmt/format.h>

namespace tiny_rpc
{
    enum class LogLevel
    {
        Debug = 1,
        Info = 2,
        Error = 3
    };
    
    class LogEvent
    {
    public:
        LogLevel get_log_evel();
        std::string get_file_name();
        void print_log();
    private:
        std::string m_file_name; // 文件名
        std::string m_file_line; // 行号 
        int m_time; // 当前时间
        int m_pid; // 进程号
        int m_thread_pid; // 线程号
        LogLevel m_log_evel; // 日志级别
    };
}
#endif