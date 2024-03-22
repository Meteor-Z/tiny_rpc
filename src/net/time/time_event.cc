#include "net/time/time_event.h"
#include "common/log.h"
#include "common/utils.h"
#include "fmt/core.h"

namespace rpc {
int64_t TimerEvent::get_arrive_time() const noexcept { return m_arrive_time; }

bool TimerEvent::is_cancel() const noexcept { return m_is_cancel; }

bool TimerEvent::is_repeat() const noexcept { return m_is_repeat; }

void TimerEvent::set_cancel(bool value) { m_is_cancel = value; };

std::function<void()> TimerEvent::get_callback() { return m_task; }

void TimerEvent::reset_arrive_time() {
    m_arrive_time = rpc::utils::get_now_ms() + m_interval;
    DEBUG_LOG(fmt::format("success create event, in execute at {}", m_arrive_time));
}

TimerEvent::TimerEvent(int64_t interval, bool repeat, std::function<void()> task)
    : m_interval(interval), m_is_repeat(repeat), m_task(task) {
    // 执行实现 = 当前时间 + 间隔点
    m_arrive_time = rpc::utils::get_now_ms() + m_interval;
    // INFO_LOG(fmt::format("success create time event, wille excute at {}", m_arrive_time));
    fmt::println("success create time event, will excute at {}", m_arrive_time);
}

} // namespace rpc