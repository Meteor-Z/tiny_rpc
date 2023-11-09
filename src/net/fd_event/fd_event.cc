#include <chrono>
#include <cstring>
#include <sys/epoll.h>
#include <fcntl.h>
#include "net/fd_event/fd_event.h"

namespace rpc {
FdEvent::FdEvent(int fd) : m_fd(fd) {
    std::memset(&m_listen_events, 0, sizeof(m_listen_events));
}

FdEvent::FdEvent() {}

FdEvent::~FdEvent() { memset(&m_listen_events, 0, sizeof(m_listen_events)); }

int FdEvent::get_fd() const noexcept { return m_fd; }

epoll_event FdEvent::get_epoll_event() const noexcept { return m_listen_events; }

// TODO:了解这个函数
void FdEvent::set_no_block() {
    int is_block = fcntl(m_fd, F_GETFL, 0);
    if (is_block & O_NONBLOCK) {
        return;
    }
    fcntl(m_fd, F_SETFL, is_block | O_NONBLOCK);
}

// 如果是读事件，那么就执行读回掉函数，否则，就是写回调函数
std::function<void()> FdEvent::handler(TriggerEvent event_type) {
    if (event_type == TriggerEvent::IN_EVENT) {
        return m_read_callback;
    }
    return m_write_callback;
}

void FdEvent::listen(TriggerEvent event_type, std::function<void()> callback) {
    if (event_type == TriggerEvent::IN_EVENT) {
        m_listen_events.events |= EPOLLIN;
        m_read_callback = callback;
    } else {
        m_listen_events.events |= EPOLLOUT;
        m_write_callback = callback;
    }
    m_listen_events.data.ptr = this;
}

void FdEvent::cancel(TriggerEvent type_event) {
    if (type_event == TriggerEvent::IN_EVENT) {
        m_listen_events.events &= (~EPOLLIN);
    } else {
        m_listen_events.events &= (~EPOLLOUT);
    }
}
} // namespace rpc