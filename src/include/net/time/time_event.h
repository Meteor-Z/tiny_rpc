/*
time_event.h
在这个定时任务中执行。
如果当前事件 now_time > arrive_time, 这时候就会执行这个定时任务
必须在arrive_time当前事件直接返回，在epoll实例中直接返回出去。

*/
#ifndef RPC_NET_TIME_EVENT_H
#define RPC_NET_TIME_EVENT_H

#include <cstdint>
#include <functional>
#include <memory>

namespace rpc {
class TimerEvent {
public:
    TimerEvent(int64_t interval, bool repeat, std::function<void()> task);
    int64_t get_arrive_time() const noexcept;
    bool is_cancel() const noexcept;
    bool is_repeat() const noexcept;
    void set_cancel(bool value);
    std::function<void()> get_callback();
    void reset_arrive_time();

private:
    int64_t m_arrive_time;        // 时间点 单位是 ms
    int64_t m_interval;           // 事件间隔 单位是 ms
    bool m_is_repeat { false };   // 是否重复
    bool m_is_cancel { false };   // 是否取消这个定时任务
    std::function<void()> m_task; // 当前回调函数
};
} // namespace rpc

#endif
