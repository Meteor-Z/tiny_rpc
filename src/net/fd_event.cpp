#include "net/fd_event.hpp"
#include <cstring>
#include <sys/epoll.h>
namespace rpc {
    Fd_Event::Fd_Event(int fd) : m_fd(fd) { std::memset(&m_listen_events, 0, sizeof(m_listen_events)); }
   
    Fd_Event::Fd_Event() {}

    Fd_Event::~Fd_Event() { memset(&m_listen_events, 0, sizeof(m_listen_events)); }
    
    int Fd_Event::get_fd() const noexcept{ return m_fd; }
    
    epoll_event Fd_Event::get_epoll_event() const noexcept { return m_listen_events; }
    
    std::function<void()> Fd_Event::handler(TriggerEvent event_type) {
        if (event_type == TriggerEvent::IN_EVENT) return m_read_callback;
        return m_write_callback;
    }

    void Fd_Event::listen(TriggerEvent event_type, std::function<void()> callback) {
        if (event_type == TriggerEvent::IN_EVENT) {
            m_listen_events.events |= EPOLLIN;
            m_read_callback = callback;
        } else {
            m_listen_events.events |= EPOLLOUT;
            m_write_callback = callback; 
        } 
         m_listen_events.data.ptr = this;
    }    
}