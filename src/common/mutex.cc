#include "common/mutex.h"
#include <pthread.h>

namespace rpc {
Mutex::Mutex() { pthread_mutex_init(&m_mutex, nullptr); }

Mutex::~Mutex() { pthread_mutex_destroy(&m_mutex); }

void Mutex::lock() { pthread_mutex_lock(&m_mutex); }

void Mutex::unlock() { pthread_mutex_unlock(&m_mutex); }

pthread_mutex_t* Mutex::get_mutex() { return &m_mutex; }
} // namespace rpc
