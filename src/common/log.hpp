#ifndef RPC_COMMON_LOG_H
#define RPC_COMMON_LOG_H
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
        Unknown = -1,
        Debug = 1,
        Info = 2,
        Error = 3,
    };

    class Logger
    {
    public:
        Logger(LogLevel log_level);
        void push_log(const std::string& message);
        static std::shared_ptr<Logger> get_global_logger();
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
        std::string m_file_name { }; // 文件名
        std::string m_file_line { }; // 行号 
        int m_time { 0 }; // 当前时间
        int m_pid { 0 }; // 进程号
        int m_thread_pid { 0 }; // 线程号
        LogLevel m_log_level; // 日志级别
        std::shared_ptr<Logger> m_logger { nullptr }; // 日志器
    };

    template<typename ... Args>
    void DEBUG_LOG(Args... args) 
    {
        std::stringstream ssin;
        (ssin << ... << args); // 折叠表达式
        std::unique_ptr<rpc::LogEvent> ptr = std::make_unique<rpc::LogEvent>(rpc::LogLevel::Debug);
        std::string message = ptr->get_log();
        std::string temp;
        ssin >> temp;
        message += temp;
        rpc::Logger::get_global_logger() -> push_log(message); // 将log 推入到队列中
        rpc::Logger::get_global_logger() -> log(); // 得到log

    }
    std::string loglevel_to_string(LogLevel loglevel);
    LogLevel string_to_loglevel(const std::string& loglevel);
    
    
    // info_log
    template<typename ... Args>
    void INFO_LOG(Args... args) 
    {
        std::stringstream ssin;
        (ssin << ... << args); // 折叠表达式

        // 这里有可能出现内存泄露
        std::unique_ptr<rpc::LogEvent> ptr = std::make_unique<rpc::LogEvent>(rpc::LogLevel::Info);
        std::string message = ptr->get_log();
        std::string temp;
        ssin >> temp;
        message += temp; 
        rpc::Logger::get_global_logger() -> push_log(message); // 将log 推入到队列中
        rpc::Logger::get_global_logger() -> log(); // 得到log

    }
    std::string loglevel_to_string(LogLevel loglevel);
    LogLevel string_to_loglevel(const std::string& loglevel);
}
#endif