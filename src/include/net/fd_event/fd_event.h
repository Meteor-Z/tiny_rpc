/**
 * @file fd_event.h
 * @author lzc (liuzechen.coder@qq.com)
 * @brief 对文件描述符进行了相关封装
 * @version 0.1
 * @date 2024-03-11
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#ifndef RPC_NET_FD_EVENT_FD_EVENT_H
#define RPC_NET_FD_EVENT_FD_EVENT_H

#include <sys/epoll.h>
#include <functional>

namespace rpc {
/**
 * @brief 封装的 Fd_Event
 *
 */
class FdEvent {
public:
    /**
     * @brief 触发的可读事件
     *
     */
    enum class TriggerEvent {
        IN_EVENT = EPOLLIN,    ///< epollin
        OUT_EVENT = EPOLLOUT,  ///< epollout
        ERROR_EVENT = EPOLLERR ///< epollerr
    };

    /**
     * @brief Construct a new Fd Event object
     *
     * @param fd 文件描述符
     */
    FdEvent(int fd);

    /**
     * @brief Construct a new Fd Event object
     *
     */
    FdEvent() = default;

    // FdEvent(const FdEvent&) = delete;
    // FdEvent(FdEvent&&) = delete;
    /**
     * @brief Destroy the Fd Event object
     *
     */
    ~FdEvent();

    // FdEvent& operator=(const FdEvent&) = delete;
    // FdEvent& operator=(FdEvent&&) = delete;

    /**
     * @brief 处理相关事件
     *
     * @param event_type 事件类型
     * @return std::function<void()> 返回要执行的函数
     */
    std::function<void()> handler(TriggerEvent event_type);

    /**
     * @brief 设置监听事件
     *
     * @param event_type 事件类型
     * @param callback 要执行的函数
     * @param error_callback 错误的时候要执行的函数
     */
    void listen(TriggerEvent event_type, std::function<void()> callback,
                std::function<void()> error_callback = nullptr);

    /**
     * @brief 取消监听
     *
     * @param type_event
     */
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
    int m_fd { -1 };                                    ///< 当前事件标识符
    epoll_event m_listen_events {};                     ///< epoll监听的事件
    std::function<void()> m_read_callback { nullptr };  ///< 读回掉函数
    std::function<void()> m_write_callback { nullptr }; ///< 写回掉函数
    std::function<void()> m_error_callback { nullptr }; ///< 发生错误时候的回调函数
};
} // namespace rpc
#endif