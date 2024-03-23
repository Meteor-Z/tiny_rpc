#ifndef RPC_COMMON_MUTEX_H
#define RPC_COMMON_MUTEX_H

#include <pthread.h>
namespace rpc {
template <typename T>
class ScopeMutex {
public:
    ScopeMutex(T& mutex) : m_mutex(mutex) {
        m_mutex.lock();
        m_is_lock = true;
    }
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
    Mutex();

    ~Mutex();

    void lock();

    void unlock();

    pthread_mutex_t* get_mutex();
private:
    pthread_mutex_t m_mutex {};
};
} // namespace rpc
#endif