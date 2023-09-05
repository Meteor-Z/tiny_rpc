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

namespace rpc
{   
    static std::mutex mtx;
    static std::shared_ptr<Logger> global_logger_ptr { nullptr }; // 使用智能指针了

    LogLevel Logger::get_log_level() const  { return m_set_level; }
    
    void Logger::push_log(const std::string& message) { m_buffer.emplace(message); }
    
    std::shared_ptr<Logger> Logger::get_global_logger() { return global_logger_ptr; }

    Logger::Logger(LogLevel log_level): m_set_level { log_level } {}

    LogLevel LogEvent::get_log_level() { return m_log_level; } 

    std::string LogEvent::get_file_name() { return m_file_name; }

    void Logger::init_global_logger()
    {
        std::shared_ptr<Config> config_ptr { Config::get_global_config() };
        LogLevel global_log_level = string_to_loglevel(config_ptr->get_m_log_level());
        fmt::println("初始化LogLevel {}", loglevel_to_string(global_log_level));
        global_logger_ptr.reset(new Logger(global_log_level));
    }

    void Logger::log()
    {  
        std::lock_guard<std::mutex> guard { mtx };
        while (!m_buffer.empty()) 
        {
            std::string message = m_buffer.front();
            std::cout << message << std::endl;
            m_buffer.pop();
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

    std::string LogEvent::get_log()  // 得到日志，并且进行格式化
    {
        // 得到线程号和进程号
        m_pid = rpc::utils::get_pid();
        m_thread_pid = rpc::utils::get_thread_id();
        std::time_t time_point = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto now = *std::localtime(&time_point);
        
        m_file_name = __FILE__;
        m_file_line = __LINE__;

        // 最终结果大概是这样的形式:[INFO][2023年9月3日16时21分37秒][文件名:/home/lzc/test_c++/main.cpp 行号 10]
        std::string result = fmt::format("[{}][{}年{}月{}日{}时{}分{}秒][文件名:{},行号 {},进程号: {} 线程号:{}]", loglevel_to_string(m_log_level), now.tm_year + 1900 , now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec,__FILE__, __LINE__,m_pid, m_thread_pid);
        return result;
    }
   
    
}