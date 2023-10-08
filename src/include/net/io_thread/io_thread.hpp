#ifndef RPC_NET_IO_THREAD_H
#define RPC_NET_IO_THREAD_H

#include "net/eventloop.hpp"
#include <sched.h>
#include <memory>
#include <semaphore>

namespace rpc 
{
    class IOThread 
    {
    public:     
        static void* Main(void* arg);
        void join();
        IOThread();
        ~IOThread();

        std::shared_ptr<EventLoop> get_eventloop();
        void start(); // 开始 event_loop 循环
    private:
        pid_t m_thread_id { -1 }; // 线程号
        pthread_t m_thread { -1ull }; // 句柄
        std::shared_ptr<EventLoop> m_event_loop { nullptr }; // 当前 io 线程 对象
        sem_t m_init_semphore { -1 };
        sem_t m_start_semphore { -1 };
    };
}

#endif