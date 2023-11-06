#include <cerrno>
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <memory>
#include <sys/select.h>
#include <sys/timerfd.h>
#include <mutex>
#include <fmt/core.h>
#include "net/time/timer.h"
#include "common/log.h"
#include "common/utils.h"
#include "net/time/time_event.h"

namespace rpc {
Timer::~Timer() {}
Timer::Timer() : FdEvent() {
    // 定时器
    m_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    rpc::utils::DEBUG_LOG(fmt::format("timer fd = {}", m_fd));

    // fdevent 在eventloop上进行监听 绑定一个可读事件 listen 是继承与FdEvent得到的
    listen(FdEvent::TriggerEvent::IN_EVENT, std::bind(&Timer::on_timer, this));
}

void Timer::on_timer() {
    char buf[8];
    // 处理缓冲区事件
    while (true) {
        if ((read(m_fd, buf, 8) == -1) && errno == EAGAIN) {
            break;
        }
    }

    int64_t now_time = rpc::utils::get_now_ms(); // 当前的事件

    std::vector<std::shared_ptr<TimerEvent>> temp;
    std::vector<std::pair<int64_t, std::function<void()>>> tasks;
    std::unique_lock<std::mutex> lock { m_mtx };
    auto it = m_pending_events.begin();

    for (; it != m_pending_events.end(); it++) {
        // 没有被取消
        if ((*it).first <= now_time) {
            if (!(*it).second->is_cancel()) {
                temp.push_back(it->second);
                tasks.push_back(std::make_pair((*it).second->get_arrive_time(),
                                               (*it).second->get_callback()));
            }

        } else {
            break;
        }
    }

    m_pending_events.erase(m_pending_events.begin(), it);
    lock.unlock();

    // 重复的 event 重复添加进去
    for (auto it = temp.begin(); it != temp.end(); it++) {
        if ((*it)->is_repeat()) {
            (*it)->reset_arrive_time();
            add_time_event(*it);
        }
    }

    reset_arrive_time();

    for (auto i : tasks) {
        if (i.second)
            i.second();
    }
}

void Timer::reset_arrive_time() {
    std::unique_lock<std::mutex> lock { m_mtx };
    auto temp = m_pending_events;
    lock.unlock();
    if (temp.empty()) {
        return;
    }

    int64_t now = rpc::utils::get_now_ms();
    auto it = temp.begin();
    int64_t inteval { 0 };
    if (it->second->get_arrive_time() > now) {
        inteval = it->second->get_arrive_time() - now;
    } else {
        inteval = 100;
    }

    timespec ts;
    std::memset(&ts, 0, sizeof(ts));
    ts.tv_sec = inteval / 1000;
    ts.tv_nsec = (inteval % 1000) * 1000000;

    itimerspec value;
    std::memset(&value, 0, sizeof(value));
    value.it_value = ts;

    int rt = timerfd_settime(m_fd, 0, &value, nullptr);
    if (rt != 0) {
        rpc::utils::ERROR_LOG(fmt::format("timerfd_settime error, errno = {} error = {}",
                                          errno, strerror(errno)));
    }

    rpc::utils::DEBUG_LOG(fmt::format("timer reset to {}", now + inteval));
}

void Timer::add_time_event(std::shared_ptr<TimerEvent> event) {
    // 需要加锁
    std::unique_lock<std::mutex> unique_lock { m_mtx };
    // 需不需重新设置超时事件
    bool is_reset_timerfd { false };
    if (m_pending_events.empty()) {
        is_reset_timerfd = true;
    } else {
        auto item = m_pending_events.begin();
        // 插入的时间大于之前的时间
        if ((*item).second->get_arrive_time() > event->get_arrive_time()) {
            is_reset_timerfd = true;
        }
    }
    m_pending_events.emplace(event->get_arrive_time(), event);
    unique_lock.unlock();

    if (is_reset_timerfd) {
        reset_arrive_time();
    }
}

void Timer::delete_time_event(std::shared_ptr<TimerEvent> event) {
    event->set_cancel(true);
    std::unique_lock<std::mutex> unique_lock { m_mtx };
    auto begin = m_pending_events.lower_bound(event->get_arrive_time());
    auto end = m_pending_events.upper_bound(event->get_arrive_time());
    auto it = begin;
    for (it = begin; it != end; it++) {
        if (it->second == event)
            break;
    }

    if (it != end) {
        m_pending_events.erase(it);
    }

    rpc::utils::DEBUG_LOG(
        fmt::format("success delete time_event at arrive {}", event->get_arrive_time()));
}
} // namespace rpc