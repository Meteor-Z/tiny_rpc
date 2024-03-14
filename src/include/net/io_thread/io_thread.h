/**
 * @file io_thread.h
 * @author liuzechen (liuzechen.coder@qq.com)
 * @brief 线程封装
 * @version 0.1
 * @date 2024-03-14
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef RPC_NET_IO_THREAD_H
#define RPC_NET_IO_THREAD_H

#include <sched.h>
#include <memory>
#include <semaphore.h>
#include "net/eventloop.h"

namespace rpc {
class IOThread {
public:
    IOThread();

    ~IOThread();

    /**
     * @brief 主动开始循环
     *
     */
    void start();

    // 结束
    void join();

    /**
     * @brief 得到当前的EventLoop
     * 
     * @return std::shared_ptr<EventLoop> 当前的EventLoop对象
     */
    std::shared_ptr<EventLoop> get_eventloop() const noexcept;

public:
    static void* Main(void* arg);

private:
    pid_t m_thread_id { -1 };                            ///< 线程号
    pthread_t m_thread { -1ull };                        ///< 句柄
    std::shared_ptr<EventLoop> m_event_loop { nullptr }; ///< 当前 io 线程对象
    sem_t m_init_semphore { { -1 } };                    ///< 初始化信号量
    sem_t m_start_semphore { { -1 } };                   ///< 开始的信号量
};
} // namespace rpc

#endif