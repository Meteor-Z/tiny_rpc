/**
 * @file mutex.h
 * @author lzc (liuzechen@qq.com)
 * @brief 封装的RAII mutex 库
 * @version 0.1
 * @date 2024-03-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef RPC_COMMON_MUTEX_H
#define RPC_COMMON_MUTEX_H

#include <pthread.h>

namespace rpc {
template <typename T>
class ScopeMutex {
public:
    ScopeMutex() = delete;
    explicit ScopeMutex(T& mutex) : m_mutex(mutex) {
        m_mutex.lock();
        m_is_lock = true;
    }

    ScopeMutex(T&&) = delete;
    ScopeMutex& operator=(const ScopeMutex&) = delete;
    ScopeMutex& operator=(ScopeMutex&&) = delete;

    ~ScopeMutex() {
        m_mutex.unlock();
        m_is_lock = false;
    }

    void lock() {
        if (!m_is_lock) {
            m_mutex.lock();
        }
    }

    void unlock() {
        if (m_is_lock) {
            m_mutex.unlock();
        }
    }

private:
    T& m_mutex {};
    bool m_is_lock { false };
};

class Mutex {
public:
    explicit Mutex();

    Mutex(const Mutex&) = delete;
    Mutex(Mutex&&) = delete;
    Mutex& operator=(const Mutex&) = delete;
    Mutex& operator=(Mutex&&) = delete;

    ~Mutex();

    void lock();

    void unlock();

    pthread_mutex_t* get_mutex();

private:
    pthread_mutex_t m_mutex {};
};
} // namespace rpc
#endif