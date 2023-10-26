#ifndef RPC_COMMON_LOG_H
#define RPC_COMMON_LOG_H

#include <memory>
#include <string>
#include <fmt/format.h>
#include <queue>
#include <sstream>


#include <iostream>
#include <string_view>
#include <source_location>

namespace rpc
{   
    // enum class 能够限制范围
    enum class LogLevel { 
        Unknown = -1,
        Debug = 1,
        Info = 2,
        Error = 3,
    };

    class Logger {
    public:
        static void init_global_logger();
        Logger(LogLevel log_level);
        void push_log(const std::string& message);
        static std::shared_ptr<Logger> get_global_logger();
        void log();
        LogLevel get_log_level() const;
    private:
        LogLevel m_set_level;
        std::queue<std::string> m_buffer;
    };
    
    // 日志事件
    class LogEvent {
    public:
        LogEvent(LogLevel level): m_log_level(level) { }
        LogLevel get_log_level();
        std::string get_file_name();
        std::string get_log(const std::string& file, int line); // 格式化，得到日志的信息
    private:
        std::string m_file_name { }; // 文件名
        int m_file_line { }; // 行号 
        int m_time { 0 }; // 当前时间
        int m_pid { 0 }; // 进程号
        int m_thread_pid { 0 }; // 线程号
        LogLevel m_log_level; // 日志级别
        std::shared_ptr<Logger> m_logger { nullptr }; // 日志器
    };
    
    namespace utils {
        void DEBUG_LOG(const std::string_view& old_message, const std::source_location& location = std::source_location::current());
        void INFO_LOG(const std::string_view& old_message, const std::source_location& location = std::source_location::current());
        void ERROR_LOG(const std::string_view& old_message, const std::source_location& location = std::source_location::current());
    }
    
    std::string loglevel_to_string(LogLevel loglevel);
    LogLevel string_to_loglevel(const std::string& loglevel);

}
#endif