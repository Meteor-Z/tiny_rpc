#include <cerrno>
#include <cstdint>
#include <ctime>
#include <sys/select.h>
#include <sys/timerfd.h>
#include <functional>
#include <memory>
#include <mutex>
#include "fmt/core.h"
#include "net/time/timer.h"
#include "common/log.h"
#include "common/utils.h"
#include "net/time/time_event.h"

namespace rpc {
Timer::~Timer() { INFO_LOG("~Timer()"); }

Timer::Timer() : FdEvent() {
    // 创建定时器任务，调用系统调用的非堵塞的任务
    // CLOCK_MONOTINIC：稳定时钟
    // TFD_NONBLOCK | TFD_CLOEXEC : 非堵塞
    // TFD_CLOEXEC：调用exec系统调用的时候保证不会在子进程中继续存在
    m_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    DEBUG_LOG(fmt::format("timer fd = {}", m_fd));

    // fdevent 在eventloop上进行监听 绑定一个可读事件
    // listen 继承于 FdEvent
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

    int64_t now_time = rpc::utils::get_now_ms();

    std::vector<std::shared_ptr<TimerEvent>> temp;
    
    // key：sec, value: task(function<void()>)
    std::vector<std::pair<int64_t, std::function<void()>>> tasks;

    std::unique_lock<std::mutex> lock { m_mtx };
    
    auto it = m_pending_events.begin();

    // 如果没有被取消，而且在这个时间里面，那么就加入到任务中
    for (; it != m_pending_events.end(); it++) {
        // 没有被取消
        if ((*it).first <= now_time) {
            if (!(*it).second->is_cancel()) {
                temp.push_back(it->second);
                tasks.push_back(std::make_pair((*it).second->get_arrive_time(), (*it).second->get_callback()));
            }
        } else {
            break;
        }
    }

    // begin() 到 it(iterator)之间全部删除
    m_pending_events.erase(m_pending_events.begin(), it);

    lock.unlock();

    // 重复的 event 重复添加进去
    for (auto iter = temp.begin(); iter != temp.end(); iter++) {
        if ((*iter)->is_repeat()) {
            (*iter)->reset_arrive_time();
            add_time_event(*iter);
        }
    }

    // 重新调整 arrive_time()
    reset_arrive_time();

    // 执行任务
    for (auto i : tasks) {
        if (i.second) {
            i.second();
        }
    }
}

void Timer::reset_arrive_time() {
    std::unique_lock<std::mutex> lock { m_mtx };
    auto temp = m_pending_events;
    lock.unlock();

    // 没有定时任务
    if (temp.empty()) {
        return;
    }

    int64_t now = rpc::utils::get_now_ms();
    auto it = temp.begin();
    int64_t inteval { 0 };

    // 如果超过了，那么就设置成100毫秒，直接运行这个定时任务
    if (it->second->get_arrive_time() > now) {
        inteval = it->second->get_arrive_time() - now;
    } else {
        inteval = 100;
    }

    // struct timespec {
    //     __time_t tv_sec; // 秒
    //     long tv_nesc;    // 纳秒
    // };

    timespec ts;
    std::memset(&ts, 0, sizeof(ts));

    // 转换一下秒数
    ts.tv_sec = inteval / 1000;
    ts.tv_nsec = (inteval % 1000) * 1000000;

    // struct itimerspec {
    //     struct timespec it_interval;  // 定时器的间隔
    //     struct timespec it_value;     // 定时器的初始值
    // };

    itimerspec value;
    std::memset(&value, 0, sizeof(value));
    value.it_value = ts;

    // 设置时间
    int rt = timerfd_settime(m_fd, 0, &value, nullptr);

    if (rt != 0) {
        ERROR_LOG(fmt::format("timerfd_settime error, errno = {} error = {}", errno, strerror(errno)));
    }

    DEBUG_LOG(fmt::format("timer reset to {}", now + inteval));
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
        // 插入的时间大于之前的时间 如果要早，那么定时任务的事件就要修改
        if ((*item).second->get_arrive_time() > event->get_arrive_time()) {
            is_reset_timerfd = true;
        }
    }

    // 插入到 队列之中
    m_pending_events.emplace(event->get_arrive_time(), event);
    unique_lock.unlock();

    if (is_reset_timerfd) {
        reset_arrive_time();
    }
}

void Timer::delete_time_event(std::shared_ptr<TimerEvent> event) {
    // 二分查找秒数
    // 设置不会触发
    event->set_cancel(true);

    std::unique_lock<std::mutex> unique_lock { m_mtx };

    auto begin = m_pending_events.lower_bound(event->get_arrive_time());
    auto end = m_pending_events.upper_bound(event->get_arrive_time());
    auto it = begin;
    for (it = begin; it != end; it++) {
        if (it->second == event) {
            break;
        }
    }

    if (it != end) {
        m_pending_events.erase(it);
    }
    // 不能删！
    unique_lock.lock();

    DEBUG_LOG(fmt::format("success delete time_event at arrive {}", event->get_arrive_time()));
}
} // namespace rpc