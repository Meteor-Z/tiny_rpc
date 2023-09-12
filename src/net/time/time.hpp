#ifndef RPC_NET_TIME_H
#define RPC_NET_TIME_H

#include "time_event.hpp"
#include "../src/net/fd_event.hpp"
#include <map>
#include <mutex>

namespace rpc
{
    // 这是一个定时器
    class Timer: public Fd_Event
    {
    public:
        Timer();
        ~Timer();
        void add_time_event(TimerEvent::s_ptr event);
        void delete_time_event(TimerEvent::s_ptr event);
        void on_time();
    private:
        void reset_arrive_time();
    private:
        
        std::multimap<int64_t, TimerEvent::s_ptr> m_pending_events;
        std::mutex m_mtx;
    };
}
#endif