#ifndef RPC_NET_TIME_EVENT_H
#define RPC_NET_TIME_EVENT_H

#include <cstdint>
#include <functional>
#include <memory>

namespace rpc {
    class TimerEvent {
    public:
        using s_ptr = std::shared_ptr<TimerEvent>;
        TimerEvent(int64_t interval, bool repeat, std::function<void()> task);
        int64_t get_arrive_time();
        bool is_cancel();
        bool is_repeat();
        void set_cancel(bool value);
        std::function<void()> get_callback();
        void reset_arrive_time();
    private:
        int64_t m_arrive_time; // 单位是ms
        int64_t m_interval; // 单位是ms
        bool m_is_repeat { false };
        bool m_is_cancel { false };

        std::function<void()> m_task;
    };
}

#endif
