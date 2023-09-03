#ifndef TINY_RPC_COMMON_LOG_H
#define TINY_RPC_COMMON_LOG_H
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <fmt/format.h>
#include <queue>
#include <sstream>

namespace rpc
{   
    enum class LogLevel
    {
        Debug = 1,
        Info = 2,
        Error = 3
    };

    class Logger
    {
    public:
        void push_log(const std::string& message);
        static Logger* get_global_logger();
        void log();
    private:
        LogLevel m_set_level;
        std::queue<std::string> m_buffer;

    };
    
    // 日志事件
    class LogEvent
    {
    public:
        LogEvent(LogLevel level): m_log_level(level) {}
        LogLevel get_log_level();
        std::string get_file_name();
        std::string get_log(); // 格式化，得到日志的信息
    private:
        std::string m_file_name; // 文件名
        std::string m_file_line; // 行号 
        int m_time; // 当前时间
        int m_pid; // 进程号
        int m_thread_pid; // 线程号
        LogLevel m_log_level; // 日志级别
        std::shared_ptr<Logger> m_logger; // 日志器
    };

    template<typename ... Args>
    void DEBUG_BLOG(Args... args) 
    {
        std::stringstream ssin;
        (ssin << ... << args); // 折叠表达式
        std::string message = (new rpc::LogEvent(rpc::LogLevel::Debug)) ->get_log();
        std::string temp;
        ssin >> temp;
        message += temp;
        rpc::Logger::get_global_logger() -> push_log(message);
        rpc::Logger::get_global_logger() -> log();

    }

    
}
#endif