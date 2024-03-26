/**
 * @file timer.h
 * @author liuzechen (liuzechen.coder@qq.com)
 * @brief 定时器的重要文件
 * @version 0.1
 * @date 2024-03-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#ifndef RPC_NET_TIMER_H
#define RPC_NET_TIMER_H

#include <memory>
#include <mutex>
#include <map>
#include "net/fd_event/fd_event.h"
#include "net/time/time_event.h"

namespace rpc {
class Timer : public FdEvent {
public:
    Timer();
    ~Timer();

    Timer(const Timer&) = delete;
    Timer(Timer&&) = delete;
    Timer& operator=(const Timer&) = delete;
    Timer& operator=(Timer&&) = delete;

    /**
     * @brief 添加事件
     *
     * @param event 事件
     */
    void add_time_event(std::shared_ptr<TimerEvent> event);

    /**
     * @brief 删除定时事件
     *
     * @param event 事件
     */
    void delete_time_event(std::shared_ptr<TimerEvent> event);

    /**
     * @brief 发生相关事件的时候才会执行
     *
     */
    void on_timer();

private:
    /**
     * @brief 重新设置时间
     *
     */
    void reset_arrive_time();

private:
    // 父类继承下来的成员函数
    //  int m_fd { -1 };                                    // 当前事件标识符
    // epoll_event m_listen_events {};                      // epoll事件
    // std::function<void()> m_read_callback { nullptr };   // 读回掉函数
    // std::function<void()> m_write_callback { nullptr };  // 写回掉函数

    // std::multimap<key(arrive_time), time_event> m_pending_events;
    std::multimap<int64_t, std::shared_ptr<TimerEvent>>
        m_pending_events; ///< TimerEvent的集合，因为key可能是重复的，那么就用multimap可重复的事件

    std::mutex m_mtx; // 锁
};
} // namespace rpc
#endif