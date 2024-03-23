#include <cassert>
#include <cstddef>
#include <cstdio>
#include <ctime>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>
#include <sys/select.h>
#include <chrono>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>
#include <sstream>
#include "common/mutex.h"
#include "fmt/core.h"
#include "common/log.h"
#include "common/log_config.h"
#include "common/utils.h"
#include "net/eventloop.h"

namespace rpc {
static std::mutex pop_log_mtx;
static std::mutex push_log_mtx;

// 全局 Logger
static std::shared_ptr<Logger> global_logger_ptr { nullptr };

LogLevel Logger::get_log_level() const { return m_set_level; }

std::shared_ptr<Logger> Logger::GET_GLOBAL_LOGGER() { return global_logger_ptr; }

void Logger::sync_loop() {
    // 同步 m_buffer 到 async_logger 到buffer尾部

    std::unique_lock<std::mutex> lock { m_mutex };
    std::vector<std::string> tmp_vec = m_buffer;
    m_buffer.swap(tmp_vec);
    lock.unlock();

    // 交换到 tmp_vec中
    if (!tmp_vec.empty()) {
        m_async_logger->push_log_buffer(tmp_vec);
    }
}

Logger::Logger(LogLevel log_level) : m_set_level { log_level } {

    // m_async_logger =
    // std::make_shared<AsyncLogger>(LogConfig::GET_GLOBAL_CONFIG()->m_file_path,
    //                                                LogConfig::GET_GLOBAL_CONFIG()->m_file_name,
    //                                                LogConfig::GET_GLOBAL_CONFIG()->m_file_max_size);

    // m_timer_evnet =
    // std::make_shared<TimerEvent>(LogConfig::GET_GLOBAL_CONFIG()->m_log_sync_inteval,
    // true,
    //                                              std::bind(&Logger::sync_loop, this));

    // // 添加到日志器里面，让他去执行
    // EventLoop::Get_Current_Eventloop()->add_timer_event(m_timer_evnet);
}

void Logger::init() {
    m_async_logger =
        std::make_shared<AsyncLogger>(LogConfig::GET_GLOBAL_CONFIG()->m_file_path,
                                      LogConfig::GET_GLOBAL_CONFIG()->m_file_name,
                                      LogConfig::GET_GLOBAL_CONFIG()->m_file_max_size);

    m_timer_evnet =
        std::make_shared<TimerEvent>(LogConfig::GET_GLOBAL_CONFIG()->m_log_sync_inteval,
                                     true, std::bind(&Logger::sync_loop, this));

    // 添加到日志器里面，让他去执行
    EventLoop::Get_Current_Eventloop()->add_timer_event(m_timer_evnet);
}

LogLevel LogEvent::get_log_level() const noexcept { return m_log_level; }

std::string LogEvent::get_file_name() const noexcept { return m_file_name; }

// 这里也要加锁
void Logger::push_log(const std::string& message) {
    std::lock_guard<std::mutex> guard { push_log_mtx };
    m_buffer.push_back(message);
}

void Logger::INIT_GLOBAL_LOGGER() {
    std::shared_ptr<LogConfig> config_ptr { LogConfig::GET_GLOBAL_CONFIG() };

    LogLevel global_log_level = string_to_loglevel(config_ptr->get_m_log_level());
    fmt::println("init LogLevel success..{}", loglevel_to_string(global_log_level));
    global_logger_ptr = std::make_shared<Logger>(global_log_level);
    //  初始化
    global_logger_ptr->init();
}

void Logger::log() {

    std::vector<std::string> tmp_vec {};

    std::unique_lock<std::mutex> lock { m_mutex };

    tmp_vec.swap(m_buffer);

    lock.unlock();

    if (!tmp_vec.empty()) {
        m_async_logger->push_log_buffer(tmp_vec);
    }
}

// namespace utils {

// void DEBUG_LOG(std::string_view message, const std::source_location &location) {
//     if (rpc::Logger::GET_GLOBAL_LOGGER()->get_log_level() <=
//         rpc::LogLevel::Debug) {
//         std::unique_ptr<rpc::LogEvent> ptr =
//             std::make_unique<rpc::LogEvent>(rpc::LogLevel::Debug);

//         std::string new_message =
//             ptr->get_log(location.file_name(), location.line()) + " " +
//             std::string{message};

//         rpc::Logger::GET_GLOBAL_LOGGER()->push_log(
//             new_message);                        // 将log 推入到队列中
//         rpc::Logger::GET_GLOBAL_LOGGER()->log(); // 得到log
//     }
// }

// void INFO_LOG(std::string_view message, const std::source_location &location) {
//     if (rpc::Logger::GET_GLOBAL_LOGGER()->get_log_level() <=
//         rpc::LogLevel::Info) {
//         std::unique_ptr<rpc::LogEvent> ptr =
//             std::make_unique<rpc::LogEvent>(rpc::LogLevel::Info);

//         std::string new_message =
//             ptr->get_log(location.file_name(), location.line()) + " " +
//             std::string{message};

//         rpc::Logger::GET_GLOBAL_LOGGER()->push_log(new_message);
//         rpc::Logger::GET_GLOBAL_LOGGER()->log(); // 得到log
//     }
// }

// void ERROR_LOG(std::string_view message, const std::source_location &location) {
//     if (rpc::Logger::GET_GLOBAL_LOGGER()->get_log_level() <=
//         rpc::LogLevel::Error) {
//         std::unique_ptr<rpc::LogEvent> ptr =
//             std::make_unique<rpc::LogEvent>(rpc::LogLevel::Error);
//         std::string new_message =
//             ptr->get_log(location.file_name(), location.line()) + " " +
//             std::string{message};
//         rpc::Logger::GET_GLOBAL_LOGGER()->push_log(
//             new_message);                        // 将log 推入到队列中
//         rpc::Logger::GET_GLOBAL_LOGGER()->log(); // 得到log
//     }
// }
// } // namespace utils

std::string loglevel_to_string(rpc::LogLevel loglevel) {
    if (loglevel == rpc::LogLevel::Debug) {
        return "Debug";
    } else if (loglevel == rpc::LogLevel::Info) {
        return "Info";
    } else if (loglevel == rpc::LogLevel::Error) {
        return "Error";
    } else {
        return "Unkown";
    }
}

rpc::LogLevel string_to_loglevel(const std::string& log_level) {
    if (log_level == "Debug") {
        return rpc::LogLevel::Debug;
    } else if (log_level == "Info") {
        return rpc::LogLevel::Info;
    } else if (log_level == "Error") {
        return rpc::LogLevel::Error;
    } else {
        return rpc::LogLevel::Unknown;
    }
}

std::string rpc::LogEvent::get_log(const std::string& file_name, int file_line) {
    m_pid = rpc::utils::get_pid();
    m_thread_pid = rpc::utils::get_thread_id();

    std::time_t time_point =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    auto now = *std::localtime(&time_point);

    m_file_name = file_name;
    m_file_line = file_line;

    // 最终结果大概是这样的形式:[INFO][2023年9月3日16时21分37秒][文件名:/home/lzc/test_c++/main.cpp
    // 行号 10]
    std::string result =
        fmt::format("[{}][{}年{}月{}日{}时{}分{}秒][文件名 {}:{}, 进程号: {} "
                    "线程号:{}]",
                    loglevel_to_string(m_log_level), now.tm_year + 1900, now.tm_mon + 1,
                    now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec, m_file_name,
                    m_file_line, m_pid, m_thread_pid);
    return result;
}

void* AsyncLogger::Loop(void* arg) {
    AsyncLogger* logger = reinterpret_cast<AsyncLogger*>(arg);

    pthread_cond_init(&logger->m_condtion, NULL);

    sem_post(&logger->m_sempahore);

    while (true) {
        ScopeMutex<Mutex> lock(logger->m_mutex);
        while (logger->m_buffer.empty()) {
            // printf("begin pthread_cond_wait back \n");
            pthread_cond_wait(&(logger->m_condtion), logger->m_mutex.get_mutex());
        }
        // printf("pthread_cond_wait back \n");

        std::vector<std::string> tmp;
        tmp.swap(logger->m_buffer.front());
        logger->m_buffer.pop();

        lock.unlock();

        timeval now;
        gettimeofday(&now, NULL);

        struct tm now_time;
        localtime_r(&(now.tv_sec), &now_time);

        const char* format = "%Y%m%d";
        char date[32];
        strftime(date, sizeof(date), format, &now_time);

        if (std::string(date) != logger->m_date) {
            logger->m_number = 0;
            logger->m_reopen_flag = true;
            logger->m_date = std::string(date);
        }
        if (logger->m_file_handler == nullptr) {
            logger->m_reopen_flag = true;
        }

        std::stringstream ss;
        ss << logger->m_file_path << logger->m_file_name << "_" << std::string(date)
           << "_log.";
        std::string log_file_name = ss.str() + std::to_string(logger->m_number);

        if (logger->m_reopen_flag) {
            if (logger->m_file_handler) {
                fclose(logger->m_file_handler);
            }
            logger->m_file_handler = fopen(log_file_name.c_str(), "a");
            logger->m_reopen_flag = false;
        }

        if (ftell(logger->m_file_handler) > logger->m_file_max_size) {
            fclose(logger->m_file_handler);

            log_file_name = ss.str() + std::to_string(logger->m_number++);
            logger->m_file_handler = fopen(log_file_name.c_str(), "a");
            logger->m_reopen_flag = false;
        }

        for (auto& item : tmp) {
            item += '\n';
            if (!item.empty()) {
                fwrite(item.c_str(), 1, item.size(), logger->m_file_handler);
            }
        }
        fflush(logger->m_file_handler);

        if (logger->m_stop_flag) {
            return nullptr;
        }
    }

    return nullptr;
}

rpc::AsyncLogger::AsyncLogger(const std::string& file_path, const std::string& file_name,
                              int m_file_max_size)
    : m_file_path(file_path), m_file_name(file_name), m_file_max_size(m_file_max_size) {
    fmt::println("file_path = {}, m_file_name = {}, m_file_max_size = {}", m_file_path,
                 m_file_name, m_file_max_size);
    // 信号量初始化
    sem_init(&m_sempahore, 0, 0);

    // 这个this指针传入到loop里面，也就是那个参数
    pthread_create(&m_thread, nullptr, &AsyncLogger::Loop, this);

    // 等待线程

    sem_wait(&m_sempahore);
}
void AsyncLogger::stop() { m_stop_flag = true; }

void AsyncLogger::flush() {
    if (m_file_handler) {
        fflush(m_file_handler);
    }
}

void AsyncLogger::push_log_buffer(std::vector<std::string>& vec) {
    // std::unique_lock<std::mutex> lock { m_mutex };
    ScopeMutex<Mutex> lock { m_mutex };

    m_buffer.push(vec);
    pthread_cond_signal(&m_condtion);

    lock.unlock();

    // 进行唤醒
}
} // namespace rpc