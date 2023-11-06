/*
定时器 继承于 FdEvent
TimeEvent的集合

*/
#ifndef RPC_NET_TIMER_H
#define RPC_NET_TIMER_H

#include <cstdint>
#include <map>
#include <mutex>
#include "net/fd_event/fd_event.h"
#include "net/time/time_event.h"

namespace rpc {
class Timer : public FdEvent {
public:
    Timer();
    ~Timer();
    void add_time_event(std::shared_ptr<TimerEvent> event);
    void delete_time_event(std::shared_ptr<TimerEvent> event);
    
    // 发生IO事件之后，需要执行的方法
    void on_timer();

private:
    void reset_arrive_time();

private:
    // 父类继承下来的成员函数
    //  int m_fd { -1 };                                    // 当前事件标识符
    // epoll_event m_listen_events {};                      // epoll事件
    // std::function<void()> m_read_callback { nullptr };   // 读回掉函数
    // std::function<void()> m_write_callback { nullptr };  // 写回掉函数

    // std::multimap<key(arrive_time), TimerEvent> m_pending_events;
    std::multimap<int64_t, std::shared_ptr<TimerEvent>>
        m_pending_events; // time_event的集合

    std::mutex m_mtx; // 锁
};
} // namespace rpc
#endif