#include "log.hpp"
#include "config.hpp"
#include "utils.hpp"

#include <chrono>
#include <ctime>
#include <fmt/core.h>
#include <iostream>
#include <fmt/format.h>
#include <memory>
#include <mutex>
#include <queue>
#include <source_location>

namespace rpc
{   
    static std::mutex pop_log_mtx;
    static std::mutex push_log_mtx;
    static std::shared_ptr<Logger> global_logger_ptr { nullptr }; // 使用智能指针了

    LogLevel Logger::get_log_level() const  { return m_set_level; }
    
    std::shared_ptr<Logger> Logger::get_global_logger() { return global_logger_ptr; }

    Logger::Logger(LogLevel log_level): m_set_level { log_level } {}

    LogLevel LogEvent::get_log_level() { return m_log_level; } 

    std::string LogEvent::get_file_name() { return m_file_name; }

    // 这里也要加锁
    void Logger::push_log(const std::string& message) 
    { 
        std::lock_guard<std::mutex> guard { push_log_mtx };
        m_buffer.emplace(message); 
    }

    void Logger::init_global_logger()
    {
        std::shared_ptr<Config> config_ptr { Config::get_global_config() };
        LogLevel global_log_level = string_to_loglevel(config_ptr->get_m_log_level());
        fmt::println("初始化LogLevel {}", loglevel_to_string(global_log_level));
        global_logger_ptr.reset(new Logger(global_log_level));
    }

    // 这里需要锁
    void Logger::log()
    {  
        std::lock_guard<std::mutex> lock_ptr { pop_log_mtx };
       
        while (!m_buffer.empty()) 
        {
            std::cout << m_buffer.front() << std::endl;
            m_buffer.pop();
        }
    }
    void DEBUG_LOG(const std::string_view& old_message, const std::source_location& location)
    {
        if (rpc::Logger::get_global_logger()->get_log_level() <= rpc::LogLevel::Debug)
        {
            std::unique_ptr<rpc::LogEvent> ptr = std::make_unique<rpc::LogEvent>(rpc::LogLevel::Debug);
            std::string message = ptr->get_log(location.file_name(), location.line()) + " " + std::string { old_message };
            rpc::Logger::get_global_logger() -> push_log(message); // 将log 推入到队列中
            rpc::Logger::get_global_logger() -> log(); // 得到log
        }
    }
    void INFO_LOG(const std::string_view& old_message, const std::source_location& location)
    {
        if (rpc::Logger::get_global_logger()->get_log_level() <= rpc::LogLevel::Info)
        {
            std::unique_ptr<rpc::LogEvent> ptr = std::make_unique<rpc::LogEvent>(rpc::LogLevel::Info);
            std::string message = ptr->get_log(location.file_name(), location.line()) + " " + std::string { old_message };
            rpc::Logger::get_global_logger() -> push_log(message); // 将log 推入到队列中
            rpc::Logger::get_global_logger() -> log(); // 得到log
        }
    }
    void ERROR_LOG(const std::string_view& old_message, const std::source_location& location)
    {
        if (rpc::Logger::get_global_logger()->get_log_level() <= rpc::LogLevel::Error)
        {
            std::unique_ptr<rpc::LogEvent> ptr = std::make_unique<rpc::LogEvent>(rpc::LogLevel::Error);
            std::string message = ptr->get_log(location.file_name(), location.line()) + " " + std::string { old_message };
            rpc::Logger::get_global_logger() -> push_log(message); // 将log 推入到队列中
            rpc::Logger::get_global_logger() -> log(); // 得到log
        }
    }
    std::string loglevel_to_string(LogLevel loglevel)
    {
        if (loglevel == LogLevel::Debug) return "Debug";
        if (loglevel == LogLevel::Info) return "Info";
        if (loglevel == LogLevel::Error) return "Error";
        return "Unkown";
    }

    LogLevel string_to_loglevel(const std::string& log_level)
    {
        if (log_level == "Debug") return LogLevel::Debug;
        if (log_level == "Info") return LogLevel::Info;
        if (log_level == "Error") return LogLevel::Error;
        return LogLevel::Unknown;
        
    }

    std::string LogEvent::get_log(const std::string& file_name, int file_line)  // 得到日志，并且进行格式化
    {
        // 得到线程号和进程号
        m_pid = rpc::utils::get_pid();
        m_thread_pid = rpc::utils::get_thread_id();
        std::time_t time_point = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto now = *std::localtime(&time_point);
        
        m_file_name = file_name;
        m_file_line = file_line;

        // 最终结果大概是这样的形式:[INFO][2023年9月3日16时21分37秒][文件名:/home/lzc/test_c++/main.cpp 行号 10]
        std::string result = fmt::format("[{}][{}年{}月{}日{}时{}分{}秒][文件名 {}:{}, 进程号: {} 线程号:{}]", loglevel_to_string(m_log_level), now.tm_year + 1900 , now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec, m_file_name, m_file_line, m_pid, m_thread_pid);
        return result;
    }
   
    
}