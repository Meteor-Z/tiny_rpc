/*
fd_evnet.h
将事件进行封装

*/
#ifndef RPC_NET_FD_EVENT_H
#define RPC_NET_FD_EVENT_H

#include <sys/epoll.h>
#include <functional>

namespace rpc {
/**
 * @brief 事件
 *
 */
class FdEvent {
public:
    // 触发的Event事件
    enum class TriggerEvent { IN_EVENT = EPOLLIN, OUT_EVENT = EPOLLOUT, ERROR_EVENT = EPOLLERR };

    // 将事件进行初始化，并且初始化 epoll_event
    FdEvent(int fd);
    FdEvent();
    ~FdEvent();

    // 处理各个事件，返回读事件 or 写事件
    std::function<void()> handler(TriggerEvent event_type);

    // 监听
    void listen(TriggerEvent event_type, std::function<void()> callback,
                std::function<void()> error_callback = nullptr);

    // 取消监听
    void cancel(TriggerEvent type_event);

    // 返回事件标识符
    int get_fd() const noexcept;

    /**
     * @brief 设置成非阻塞模式
     * 
     */
    void set_no_block();

    // 返回 epoll 事件
    epoll_event get_epoll_event() const noexcept;

    /**
     * @brief 设置错误的回调函数信息
     * 
     * @param call_back 
     */
    void set_error_callback(std::function<void()> call_back);

protected:
    int m_fd { -1 };                                    // 当前事件标识符
    epoll_event m_listen_events {};                     // epoll事件
    std::function<void()> m_read_callback { nullptr };  // 读回掉函数
    std::function<void()> m_write_callback { nullptr }; // 写回掉函数
    std::function<void()> m_error_callback { nullptr }; ///< 发生错误时候的回调函数
};
} // namespace rpc
#endif