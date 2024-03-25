/**
 * @file eventloop.h
 * @author liuzechen (liuzechen.coder@qq.com)
 * @brief 主从Reactor模块的重要文件
 * @version 0.1
 * @date 2024-03-11
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef RPC_NET_EVENTLOOP_H
#define RPC_NET_EVENTLOOP_H

#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include "net/fd_event/fd_event.h"
#include "net/time/timer.h"
#include "net/wakeup_fd_event.h"
#include "time/time_event.h"

namespace rpc {
/**
 * @brief EventtLoop，事件循环类
 *
 */
class EventLoop : public std::enable_shared_from_this<EventLoop> {

public:
    // 得到全局对象EventLoop
    static std::shared_ptr<EventLoop> Get_Current_Eventloop();

public:
    /**
     * @brief Construct a new Event Loop object
     *
     */
    /**
     * @brief Construct a new Event Loop object
     *
     */
    EventLoop();

    ~EventLoop();

    /**
     * @brief loop循环启动
     *
     */
    void loop();

    /**
     * @brief 唤醒EventLoop
     *
     */
    void wake_up();

    /**
     * @brief 将EventLoop停下来，
     * @note 不过一般不会停止，因为事件一直在循环
     */
    void stop();

    /**
     * @brief 增加事件循环
     *
     * @param event 事件
     */
    void add_epoll_event(std::shared_ptr<FdEvent> event);

    /**
     * @brief 删除epoll_event事件
     *
     * @param event 事件
     */
    void delete_epoll_event(std::shared_ptr<FdEvent> event);

    /**
     * @brief // 是否是当前函数线程
     *
     * @return true 是当前线程函数
     * @return false 不是当前线程函数
     */
    bool is_in_current_loop_thread();

    // 添加定时任务
    void add_timer_event(std::shared_ptr<TimerEvent> shard_ptr);

    /**
     * @brief 是否在循环
     *
     * @return true
     * @return false
     */
    bool is_looping() const noexcept;

    int get_pending_tasks_size();

private:
    // 处理wake_up事件
    void deal_wake_up();

    /**
     * @brief 添加事件
     *
     * @param event 事件
     */
    void add_to_epoll(std::shared_ptr<FdEvent> event);

    /**
     * @brief 删除当前事件
     *
     * @param event
     */
    void delete_from_epoll(std::shared_ptr<FdEvent> event);

    //  初始化wakeup_fd
    void init_wakeup_fd_event();

    void init_timer();

    /**
     * @brief 添加相关事件
     *
     * @param task 任务
     * @param is_wake_up 是否快速唤醒epoll_wait,相当于立刻加入到其中
     */
    void add_task(std::function<void()> task, bool is_wake_up = false);

private:
    pid_t m_thread_id { 0 }; ///< 线程号
    int m_epoll_fd { 0 };    ///< epoll文件描述符句柄
    int m_wakeup_fd { 0 };   ///< 唤醒的文件描述符
    std::shared_ptr<WakeUpFdEvent> m_wakeup_fd_event { nullptr }; ///< 要进行唤醒
    bool m_stop_flag { false };                        ///< eventloop是否暂停？
    std::set<int> m_listen_fds;                        ///< 正在监听的套接字
    std::queue<std::function<void()>> m_pending_tasks; ///< 待执行的任务队列。
    std::mutex m_mtx;                                  ///< 互斥锁
    std::shared_ptr<Timer> m_timer { nullptr };        ///< 定时任务
    bool m_is_looping { false };                       ///< 是否正在loop中
};
} // namespace rpc

#endif