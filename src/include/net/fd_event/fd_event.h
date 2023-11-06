/*
fd_evnet.h
将事件进行封装

*/
#ifndef RPC_NET_FD_EVENT_H
#define RPC_NET_FD_EVENT_H

#include <functional>
#include <sys/epoll.h>

namespace rpc {
class FdEvent {
public:
    // 触发的Event事件
    enum class TriggerEvent {
        IN_EVENT = EPOLLIN,
        OUT_EVENT = EPOLLOUT,
    };

    // 将事件进行初始化，并且初始化 epoll_event
    FdEvent(int fd);
    FdEvent();
    ~FdEvent();

    // 处理各个事件，返回读事件 or 写事件
    std::function<void()> handler(TriggerEvent event_type);
    
    // 监听
    void listen(TriggerEvent event_type, std::function<void()> callback);

    // 返回事件标识符
    int get_fd() const noexcept;
    
    // 返回 epoll 事件
    epoll_event get_epoll_event() const noexcept;

protected:
    int m_fd { -1 };                                    // 当前事件标识符
    epoll_event m_listen_events {};                     // epoll事件
    std::function<void()> m_read_callback { nullptr };  // 读回掉函数
    std::function<void()> m_write_callback { nullptr }; // 写回掉函数
};
} // namespace rpc
#endif