#ifndef RPC_NET_EVENTLOOP_H
#define RPC_NET_EVENTLOOP_H

#include "fd_event.hpp"
#include "time/time_event.hpp"
#include "wakeup_fd_event.hpp"
#include "net/time/timer.hpp"

#include <memory>
#include <thread>
#include <set>
#include <queue>
#include <functional>
#include <mutex>

namespace rpc 
{
    class EventLoop 
    {
    public:
        EventLoop();
        ~EventLoop();
        void loop();
        void wake_up();
        void stop();
        void add_epoll_event(Fd_Event* event);
        void delete_epoll_event(Fd_Event* event);
        bool is_in_loop_thread();
        void add_task(std::function<void()> task, bool is_wake_up = false);
        void add_timer_event(rpc::TimerEvent::s_ptr shard_ptr); //添加定时任务
    private:
        void deal_wake_up();
        void add_to_epoll(Fd_Event* event);
        void delete_from_epoll(Fd_Event* event);
        void init_wakeup_fd_event();
        void init_timer();
    private:
        pid_t m_thread_id { 0 }; // 线程号
        int m_epoll_fd { 0 }; // epoll 句柄 这里是 epoll 例程
        int m_wakeup_fd { 0 }; // 唤醒的一个文件标识符
        WakeUPEvent* m_wakeup_fd_event { nullptr }; // 唤醒的事件
        bool m_stop_flag { false }; 
        std::set<int> m_listen_fds; // 监听的套接字，存入的是文件描述符
        std::queue<std::function<void()>> m_pending_tasks; // 好好好，很c++ style
        std::mutex m_mtx; // 这个是锁
        Timer* m_timer { nullptr };
    };
}

#endif