#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include "net/eventloop.h"
#include "net/fd_event/fd_event.h"
#include "net/time/time_event.h"
#include "net/wakeup_fd_event.h"
#include "common/utils.h"
#include "common/log.h"

namespace rpc {
static thread_local std::unique_ptr<EventLoop> thread_current_eventloop {
    nullptr
}; ///< 每一个线程独有的EventLoop对象

static constexpr int G_EPOLL_MAX_TIMEOUT { 10000 }; ///< epoll 最大的事件间隔

static constexpr int G_EPOLL_MAX_EVENTS { 10 }; ///< 单次最大的监听事件

bool EventLoop::is_in_current_loop_thread() {
    return m_thread_id == rpc::utils::get_thread_id();
}

void EventLoop::deal_wake_up() {}

EventLoop::EventLoop() {
    if (thread_current_eventloop != nullptr) {
        ERROR_LOG(fmt::format(
            "eventloop is not nullptr, failed to creat event loop, error info = {}",
            errno));
        exit(0);
    }

    // 得到当前 线程id
    m_thread_id = rpc::utils::get_thread_id();

    // 创建 epoll句柄
    m_epoll_fd = epoll_create(100); // 随便传入 现代操作系统已经不用管这个数字了

    // 如果申请不了
    if (m_epoll_fd == -1) {
        ERROR_LOG(fmt::format("failed to epoll_create(),error info {}", errno));
        exit(0);
    }

    // eventfd 设置成非阻塞的
    // m_wakeup_fd = eventfd(0, EFD_NONBLOCK);
    // if (m_wakeup_fd < 0) {
    //     ERROR_LOG(fmt::format("不能够创建eventfd ,error info {}", errno));
    //     exit(0);
    // }

    init_wakeup_fd_event();
    init_timer();

    INFO_LOG(fmt::format("在{}线程下成功创建", m_thread_id));

    // thread_current_eventloop = shared_from_this();
    // std::shared_ptr 不能这样使用
    // thread_current_eventloop = std::shared_ptr<EventLoop>(this) ;
}

EventLoop::~EventLoop() {
    // 先这样处理，因为这样也是线程不安全的
    DEBUG_LOG("~EventLoop");
    close(m_epoll_fd);
}

void EventLoop::init_wakeup_fd_event() {
    // 非堵塞的环形，然后加入到eventloop事件里面，然后进行唤醒
    m_wakeup_fd = eventfd(0, EFD_NONBLOCK);
    if (m_wakeup_fd < 0) {
        ERROR_LOG(fmt::format(
            "failed to create eventloop,, eventfd create error ,error info {}", errno));
        exit(1);
    }

    INFO_LOG(fmt::format("wakeup fd = {}", m_wakeup_fd));
    // m_wakeup_fd_event = new WakeUpFdEvent(m_wakeup_fd); // 事件唤醒
    m_wakeup_fd_event = std::make_shared<WakeUpFdEvent>(m_wakeup_fd);
    m_wakeup_fd_event->listen(FdEvent::TriggerEvent::IN_EVENT, [this]() {
        char buf[8];
        while (read(m_wakeup_fd, buf, 8) != -1 && errno != EAGAIN) {
        }
        DEBUG_LOG(fmt::format("read full bytes from wakeup fd {}", m_wakeup_fd));
    });

    add_epoll_event(m_wakeup_fd_event);
}

/*
先将任务队列中的人物处理掉，然后再进行处理io时间，将其加入到队列中？
*/
void EventLoop::loop() {
    // 正在 loop
    m_is_looping = true;
    while (!m_stop_flag) {
        // ------------- lock() 锁住这里就行了，下面还有地方会用到锁
        std::unique_lock<std::mutex> lock { m_mtx };
        std::queue<std::function<void()>> temp_tasks;
        m_pending_tasks.swap(temp_tasks);
        lock.unlock();
        // ---------------------

        // 在这里执行相关的函数，注意，这里是堵塞的。
        while (!temp_tasks.empty()) {
            std::function<void()> cb = temp_tasks.front();
            temp_tasks.pop();
            if (cb) {
                cb();
            }
        }

        // 等到的最大时间
        int time_out = G_EPOLL_MAX_TIMEOUT;

        epoll_event result_event[G_EPOLL_MAX_EVENTS];

        // 返回的是 数量
        int epoll_num =
            epoll_wait(m_epoll_fd, result_event, G_EPOLL_MAX_EVENTS, time_out);
        DEBUG_LOG(fmt::format("epoll_wait。。。 rt = {}", epoll_num));

        if (epoll_num < 0) {
            ERROR_LOG(fmt::format("epoll_wait error, errno = {}", errno));
        } else {
            // 将事件进行处理
            for (int i = 0; i < epoll_num; i++) {
                // trigger 意思：
                // 触发，将其转换成FdEvent,就是里面的union指针，要将其转发成FdEvent
                epoll_event trigger_event = result_event[i];
                std::shared_ptr<FdEvent> fd_event_ptr = std::make_shared<FdEvent>(
                    *static_cast<FdEvent*>(trigger_event.data.ptr));
                if (fd_event_ptr == nullptr) {
                    ERROR_LOG("fd_event = nullptr, continue");
                    continue;
                }
                // 可读事件，而且不是唤醒的，优先级不是很高
                if (trigger_event.events & EPOLLIN) {
                    DEBUG_LOG(fmt::format("fd {} trigger EPOLLIN event",
                                          fd_event_ptr->get_fd()));
                    add_task(fd_event_ptr->handler(FdEvent::TriggerEvent::IN_EVENT));
                }
                // 可写事件，不是唤醒，优先级不是很好
                if (trigger_event.events & EPOLLOUT) {
                    DEBUG_LOG(fmt::format("fd {} trigger EPOLLOUT event",
                                          fd_event_ptr->get_fd()));
                    add_task(fd_event_ptr->handler(FdEvent::TriggerEvent::OUT_EVENT));
                }

                // if (!(trigger_event.events & EPOLLIN) &&
                //     !(trigger_event.events & EPOLLOUT)) {

                //     DEBUG_LOG(fmt::format("unkonow event = {}", trigger_event.events));
                // }
                if (trigger_event.events & EPOLLERR) {

                    DEBUG_LOG(fmt::format("fd = {}, EPOLLERR", fd_event_ptr->get_fd()));
                    delete_epoll_event(fd_event_ptr);
                    if (fd_event_ptr->handler(FdEvent::TriggerEvent::ERROR_EVENT) !=
                        nullptr) {
                        add_task(fd_event_ptr->handler(FdEvent::TriggerEvent::OUT_EVENT));
                    }
                }
            }
        }
    }
}

void EventLoop::wake_up() {
    INFO_LOG("wakeup....");
    m_wakeup_fd_event->wakeup();
}

void EventLoop::init_timer() {
    m_timer = std::make_shared<Timer>();
    add_epoll_event(m_timer); // 将这个事件存存放到这个事件中
}

void EventLoop::add_timer_event(std::shared_ptr<TimerEvent> shard_ptr) {
    m_timer->add_time_event(shard_ptr);
}
bool EventLoop::is_looping() const noexcept { return m_is_looping; }

void EventLoop::stop() {
    m_stop_flag = true;
    wake_up();
}

int EventLoop::get_pending_tasks_size() { return m_pending_tasks.size(); }

std::unique_ptr<EventLoop> EventLoop::Get_Current_Eventloop() {
    if (!thread_current_eventloop) {
        thread_current_eventloop = std::make_unique<EventLoop>();
    }

    return std::move(thread_current_eventloop);
}
// 这里为啥要判断是当前线程啊
void EventLoop::add_epoll_event(std::shared_ptr<FdEvent> event) {
    if (is_in_current_loop_thread()) {
        add_to_epoll(event);
    } else {
        // 包装一层 lambda
        auto cb = [this, event]() { add_to_epoll(event); };
        add_task(cb, true);
    }
}
/// TODO: 只有是当前线程操作的时候，才会添加任务，否则包装一个任务，加入到epoll事件中
void EventLoop::delete_epoll_event(std::shared_ptr<FdEvent> event) {
    if (is_in_current_loop_thread()) {
        delete_from_epoll(event);
    } else {
        auto cb = [this, event]() { delete_from_epoll(event); };

        add_task(cb, true);
    }
}

void EventLoop::add_task(std::function<void()> task, bool is_wake_up /* = false */) {
    std::lock_guard<std::mutex> lock { m_mtx };
    m_pending_tasks.push(task);
    if (is_wake_up) {
        wake_up();
    }
}

// EPOLL_CTL_ADD: 添加一个新的文件描述符
// EPOLL_CTL_MOD: 修改一个已经在epoll描述符中的内容
// epoll_ctl: 对epoll实例中的数据进行修改。
void EventLoop::add_to_epoll(std::shared_ptr<FdEvent> event) {
    auto it = m_listen_fds.find(event->get_fd());
    int op = EPOLL_CTL_ADD;

    if (it != m_listen_fds.end()) {
        op = EPOLL_CTL_MOD;
    }

    epoll_event tmp = event->get_epoll_event();

    INFO_LOG(fmt::format("epoll_event.events = {}", (int)tmp.events));

    // 添加事件
    int rt = epoll_ctl(m_epoll_fd, op, event->get_fd(), &tmp); // 注册 添加事件
    if (rt == -1) {
        ERROR_LOG(fmt::format("failed epoll_ctl when add fd, errno={}, error={}", errno,
                              strerror(errno)));
    }

    m_listen_fds.insert(event->get_fd());
    DEBUG_LOG(fmt::format("add event success, fd[{}]", event->get_fd()));
}

// EPOLL_CTL_DEL: 删除当前文件描述符
void EventLoop::delete_from_epoll(std::shared_ptr<FdEvent> event) {
    auto it = m_listen_fds.find(event->get_fd());
    if (it == m_listen_fds.end()) {
        return;
    }

    int op = EPOLL_CTL_DEL;
    // epoll_event tmp = event->get_epoll_event();

    // 删除掉
    int rt = epoll_ctl(m_epoll_fd, op, event->get_fd(), nullptr);
    if (rt == -1) {
        ERROR_LOG(fmt::format("failed epoll_ctl when add fd, errno={}, error={}", errno,
                              strerror(errno)));
    }

    // 在监听中也将这个进行删除
    m_listen_fds.erase(event->get_fd());
    DEBUG_LOG(fmt::format("delete event success, fd[{}]", event->get_fd()));
}
} // namespace rpc