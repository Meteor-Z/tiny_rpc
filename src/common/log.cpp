#include "log.h"
#include <chrono>
#include <ctime>
#include <iostream>
namespace tiny_rpc
{
    LogLevel LogEvent::get_log_evel() { return m_log_evel; }
    std::string LogEvent::get_file_name() { return m_file_name; }
    void LogEvent::print_log() 
    {
        std::time_t time_point = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto time_now = *std::localtime(&time_point);
        std::cout << time_now.tm_year + 1900 << '/'
                  << time_now.tm_mon + 1 << '/'
                  << time_now.tm_mday << '/'
                  << time_now.tm_hour << '/'
                  << time_now.tm_min << '/'
                  << time_now.tm_sec << std::endl;
    }