/*
 日志库，将其信息打印在屏幕上，后续会将其存入硬盘上。
 最终结果大概是这样的形式:
 [INFO][2023年9月3日16时21分37秒][文件名:/home/lzc/test_c++/main.cpp行号10]

LogConfig进行初始化，

*/
#ifndef RPC_COMMON_LOG_H
#define RPC_COMMON_LOG_H

#include <fmt/format.h>
#include <iostream>
#include <memory>
#include <queue>
#include <source_location>
#include <sstream>
#include <string>
#include <string_view>

namespace rpc {
// enum class 能够限制范围
enum class LogLevel {
    Unknown = -1,
    Debug = 1,
    Info = 2,
    Error = 3,
};

/**
日志器：将内容输出出来，基本上，是一个全局变量
*/
class Logger {
public:
    Logger(LogLevel log_level);
    // 将message加入到buffer中
    void push_log(const std::string &message);
    // 这里将内容输出出来
    void log();
    LogLevel get_log_level() const;

public:
    static std::shared_ptr<Logger> GET_GLOBAL_LOGGER();
    // 每一个项目都要初始化logger
    static void INIT_GLOBAL_LOGGER();

private:
    LogLevel m_set_level;             // 设置的等级
    std::queue<std::string> m_buffer; // 缓冲区
};

// 日志事件，
class LogEvent {
public:
    LogEvent(LogLevel level) : m_log_level(level) {}
    LogLevel get_log_level();
    std::string get_file_name();
    //
    std::string get_log(const std::string &file, int line);

private:
    std::string m_file_name{};                 // 文件名
    int m_file_line{};                         // 行号
    int m_time{0};                             // 当前时间
    int m_pid{0};                              // 进程号
    int m_thread_pid{0};                       // 线程号
    LogLevel m_log_level;                      // 日志级别
    std::shared_ptr<Logger> m_logger{nullptr}; // 日志器
};

/**
old_message


*/
namespace utils {
void DEBUG_LOG(std::string_view message, const std::source_location &location =
                                             std::source_location::current());
void INFO_LOG(
    std::string_view old_message,
    const std::source_location &location = std::source_location::current());
void ERROR_LOG(std::string_view message, const std::source_location &location =
                                             std::source_location::current());
} // namespace utils

std::string loglevel_to_string(LogLevel loglevel);
LogLevel string_to_loglevel(const std::string &loglevel);

} // namespace rpc
#endif