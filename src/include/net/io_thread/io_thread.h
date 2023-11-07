#ifndef RPC_NET_IO_THREAD_H
#define RPC_NET_IO_THREAD_H

#include <sched.h>
#include <memory>
#include <semaphore>
#include "net/eventloop.h"

namespace rpc {
class IOThread {
public:
    IOThread();

    ~IOThread();

    // 开始 eventloop循环
    void start();

    // 结束
    void join();

    std::shared_ptr<EventLoop> get_eventloop() const noexcept;

public:
    static void* Main(void* arg);

private:
    pid_t m_thread_id { -1 };                            // 线程号
    pthread_t m_thread { -1ull };                        // 句柄
    std::shared_ptr<EventLoop> m_event_loop { nullptr }; // 当前 io 线程 对象
    sem_t m_init_semphore { -1 };                        // 初始化信号量
    sem_t m_start_semphore { -1 };                       // 开始的信号量
};
} // namespace rpc

#endif