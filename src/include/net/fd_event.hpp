#ifndef RPC_NET_FD_EVENT_H
#define RPC_NET_FD_EVENT_H

#include <functional>
#include <sys/epoll.h>

namespace rpc
{
    // 将这个进行封装
    class Fd_Event
    {
    public:
        enum class TriggerEvent
        {
            IN_EVENT = EPOLLIN,
            OUT_EVENT = EPOLLOUT,
        };
        Fd_Event(int fd); // 将事件进行初始化，并且初始化 epoll_event
        Fd_Event();
        ~Fd_Event();
        std::function<void()> handler(TriggerEvent event_type);
        void listen(TriggerEvent event_type, std::function<void()> callback);
        int get_fd() const noexcept;
        epoll_event get_epoll_event() const noexcept;
    protected:
        int m_fd { -1 }; 
        epoll_event m_listen_events { };
        std::function<void()> m_read_callback { nullptr };
        std::function<void()> m_write_callback { nullptr};
    };
}
#endif