/**
 * @file log.h
 * @author meteor_z (liuzechen.coder@qq.com)
 * @brief 库，提供DEBUG_LOG, ERROR_LOG, INFO_LOG 三种格式的信息
 * @version 0.1
 * @date 2023-12-24
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef RPC_COMMON_LOG_H
#define RPC_COMMON_LOG_H

#include <condition_variable>
#include <cstdio>
#include <memory>
#include <mutex>
#include <pthread.h>
#include <queue>
#include <semaphore.h>
#include <string>
#include <vector>
#include "common/mutex.h"
#include "fmt/core.h" ///< 需要这个
#include "net/time/time_event.h"

// rpc::Logger::GET_GLOBAL_LOGGER()->log();
/**
 * @brief 根据xml格式进行调整的信息格式
 *
 */
#define DEBUG_LOG(message)                                                               \
    if (rpc::Logger::GET_GLOBAL_LOGGER()->get_log_level() <= rpc::LogLevel::Debug) {     \
        std::unique_ptr<rpc::LogEvent> ptr =                                             \
            std::make_unique<rpc::LogEvent>(rpc::LogLevel::Debug);                       \
        std::string new_message =                                                        \
            ptr->get_log(__FILE__, __LINE__) + " " + std::string { message };            \
        rpc::Logger::GET_GLOBAL_LOGGER()->push_log(new_message);                         \
    }

/**
 * @brief 根据xml文件进行调整的信息格式
 *
 */
#define INFO_LOG(message)                                                                \
    if (rpc::Logger::GET_GLOBAL_LOGGER()->get_log_level() <= rpc::LogLevel::Info) {      \
        std::unique_ptr<rpc::LogEvent> ptr =                                             \
            std::make_unique<rpc::LogEvent>(rpc::LogLevel::Info);                        \
        std::string new_message =                                                        \
            ptr->get_log(__FILE__, __LINE__) + " " + std::string { message };            \
        rpc::Logger::GET_GLOBAL_LOGGER()->push_log(new_message);                         \
    }

/**
 * @brief 错误信息，根据xml调整，是最高等级的信息调试
 *
 */
#define ERROR_LOG(message)                                                               \
    if (rpc::Logger::GET_GLOBAL_LOGGER()->get_log_level() <= rpc::LogLevel::Error) {     \
        std::unique_ptr<rpc::LogEvent> ptr =                                             \
            std::make_unique<rpc::LogEvent>(rpc::LogLevel::Error);                       \
        std::string new_message =                                                        \
            ptr->get_log(__FILE__, __LINE__) + " " + std::string { message };            \
        rpc::Logger::GET_GLOBAL_LOGGER()->push_log(new_message);                         \
    }

namespace rpc {
// enum class 能够限制范围
enum class LogLevel {
    Unknown = -1,
    Debug = 1,
    Info = 2,
    Error = 3,
};
/**
 * @brief 异步的框架，开辟一个专门的线程去输出这些日志
 *
 */
class AsyncLogger {

public:
    /**
     * @brief 一直循环的loop,进行打印
     *
     * @param arg 传入的的一个AsyncLogger(), 然后强制钻换一下就行了
     * @return void*
     */
    static void* Loop(void* arg);

public:
    /**
     * @brief Construct a new Async Logger object
     *
     * @param file_path 日志输出的文件路径
     * @param file_name 日志的名称
     * @param m_file_max_size 最大的输出大小，如果大于这个大小，那么就会发生滚动，
     * .1->.2->.3这样滚动
     */
    AsyncLogger(const std::string& file_path, const std::string& file_name,
                int m_file_max_size);

    /**
     * @brief 停止
     *
     */
    void stop();

    /**
     * @brief 强制刷盘
     *
     */
    void flush();

    /**
     * @brief 将文件唤醒到日志器里面
     *
     * @param vec
     */
    void push_log_buffer(std::vector<std::string>& vec);

private:
    std::queue<std::vector<std::string>> m_buffer {}; ///< buffer
    std::string m_file_path {};                       ///< 文件路径
    std::string m_file_name {};                       ///< 文件名
    int m_file_max_size {};                           ///< 单个文件最大的大小
    sem_t m_sempahore {};                             ///< 通知的信号量

    pthread_t m_thread {};        ///< 输出日志的当前线程
    pthread_cond_t m_condtion {}; ///< 条件变量
    Mutex m_mutex {};        ///< 互斥锁，配合条件变量的使用

    std::string m_date;           ///< 上次打印日志的文件日期
    FILE* m_file_handler {};      ///< 当前文件打开的的文件句柄
    bool m_reopen_flag { false }; ///< 是否重新打开
    int m_number { 0 };           ///< 日志序号

    bool m_stop_flag { false }; ///< 是否退出
};
/**
 * @brief 日志器
 *
 */
class Logger {
public:
    /**
     * @brief Get the global logger object
     *
     * @return std::shared_ptr<Logger>
     */
    static std::shared_ptr<Logger> GET_GLOBAL_LOGGER();

    /**
     * @brief 初始化日志
     *
     */
    static void INIT_GLOBAL_LOGGER();

public:
    /**
     * @brief Construct a new Logger object
     *
     * @param log_level
     */
    Logger(LogLevel log_level);

    /**
     * @brief log打印出来
     *
     * @param message 相关信息
     */
    void push_log(const std::string& message);

    /**
     * @brief 日志输出
     *
     */
    void log();

    /**
     * @brief 初始化方法
     *
     */
    void init();

    /**
     * @brief 得到日志等级
     *
     * @return LogLevel 日志等级
     */
    LogLevel get_log_level() const;

    /**
     * @brief 定时任务，每隔一段时间，就会执行，将其buffer放到异步器中的buffer里面
     *
     */
    void sync_loop();

private:
    LogLevel m_set_level;                           ///< 设置的等级
    std::vector<std::string> m_buffer;              ///< 缓冲区
    std::shared_ptr<AsyncLogger> m_async_logger {}; ///< 异步器
    std::shared_ptr<TimerEvent> m_timer_evnet {};   ///< 定时器
    std::mutex m_mutex {};                          ///< 自定义锁
};

// 日志事件，
class LogEvent {
public:
    LogEvent(LogLevel level) : m_log_level(level) {}

    LogLevel get_log_level() const noexcept;

    std::string get_file_name() const noexcept;
    // 对于log,进行格式化
    // 最终结果大概是这样的形式:[INFO][2023年9月3日16时21分37秒][文件名:/home/lzc/test_c++/main.cc
    std::string get_log(const std::string& file, int line);

private:
    std::string m_file_name {}; // 文件名
    int m_file_line {};         // 行号
    int m_time { 0 };           // 当前时间
    int m_pid { 0 };            // 进程号
    int m_thread_pid { 0 };     // 线程号
    LogLevel m_log_level;       // 日志级别
    // std::shared_ptr<Logger> m_logger{nullptr}; // 日志器
};

/*
debug工具，std::location是缺省值，不需要填写
*/
// namespace utils {
// void DEBUG_LOG(std::string_view message,
//                const std::source_location& location = std::source_location::current());
// void INFO_LOG(std::string_view message,
//               const std::source_location& location = std::source_location::current());

// void ERROR_LOG(std::string_view message,
//                const std::source_location& location = std::source_location::current());
// } // namespace utils

std::string loglevel_to_string(LogLevel loglevel);
LogLevel string_to_loglevel(const std::string& loglevel);

} // namespace rpc
#endif