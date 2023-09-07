#ifndef RPC_NET_FD_EVENT_H
#define RPC_NET_FD_EVENT_H

#include <functional>
#include <sys/epoll.h>
namespace rpc
{
    
    class Fd_Event
    {
    public:
        enum class TriggerEvent
        {
            IN_EVENT = EPOLLIN,
            OUT_EVENT = EPOLLOUT,
        };
        Fd_Event(int fd);
        ~Fd_Event();
        std::function<void()> handler(TriggerEvent event_type);
        void listen(TriggerEvent event_type, std::function<void()> callback);
        int get_fd() const;
        epoll_event get_epoll_event();
    protected:
        int m_fd { -1 };
        epoll_event m_listen_events { };
        std::function<void()> m_read_callback { nullptr };
        std::function<void()> m_write_callback { nullptr};
    };
}
#endif