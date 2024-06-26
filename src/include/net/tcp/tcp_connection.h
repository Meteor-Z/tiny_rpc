/**
 * @file tcp_connection.h
 * @author liuzechen (liuzechen.coder@qq.com)
 * @brief TcpConnection 一次请求
 * @version 0.1
 * @date 2024-03-15
 *
 * @copyright Copyright (c) 2024
 *
 */

/*
TcpConnection()

read() -> excute() -> write()

read(): 读取客户端发来的请求，然后组装成RPC请求
excute(): 将RPC请求作为入参，执行业务逻辑得到RPC相应
write(): 将RPC相应发送到客户端
以上三个步骤一直执行。、

*/

#pragma once

#ifndef RPC_NET_TCP_TCP_CONNECTION_H
#define RPC_NET_TCP_TCP_CONNECTION_H

#include <functional>
#include <memory>
#include <utility>
#include <vector>
#include "net/eventloop.h"
#include "net/fd_event/fd_event.h"
#include "net/tcp/ipv4_net_addr.h"
#include "net/tcp/tcp_buffer.h"
#include "net/coder/abstract_coder.h"
#include "net/coder/abstract_protocol.h"

// ok
namespace rpc {
class TcpConnection {
public:
    /**
     * @brief 连接状态
     *
     */
    enum class TcpState {
        NotConnected = 1, ///< 无连接
        Connected = 2,    ///< 连接
        HalfClosing = 3,  ///< 半连接
        Closed = 4,       ///< 关闭
    };

    /**
     * @brief 区分TcpConnection的类型
     *
     */
    enum class TcpConnectionType {
        TcpConnectionByServer = 1, //< 服务端使用,客户端连接
        TcpConnectionByClient = 2, //< 客户端使用，对服务端连接
    };

public:
    // io_thread: 当前io线程
    // fd: 指向的那个套接字
    // buffer_size: 初始化buffer的大小
    // peer_addr: 地址
    TcpConnection(std::shared_ptr<EventLoop> event_loop, int fd, int buffer_size,
                  std::shared_ptr<IPv4NetAddr> local_addr,
                  std::shared_ptr<IPv4NetAddr> peer_addr,
                  TcpConnectionType type = TcpConnectionType::TcpConnectionByServer);

    ~TcpConnection();

    TcpConnection() = delete;
    TcpConnection(const TcpConnection&) = delete;
    TcpConnection(TcpConnection&&) = delete;

    TcpConnection& operator=(const TcpConnection&) = delete;
    TcpConnection& operator=(TcpConnection&&) = delete;

    /**
     * @brief 一次性将其全部读完，这个函数最终是放到了EventLoop中进行执行，如果有可读事件了，那么就会触发这个函数
     *
     */
    void on_read();

    /**
     * @brief 执行函数，根据req_id找到这些函数，然后执行这些函数
     * 
     */
    void excute();

    /**
     * @brief 发送回去
     * 
     */
    void on_write();

    void set_state(const TcpConnection::TcpState& state);

    TcpConnection::TcpState get_state() const noexcept;

    // 清除连接
    void clear();

    /**
     * @brief 这个是让其处于半关闭状态,封装shutdown这个函数
     *
     */
    void shutdown();

    void set_connection_type(TcpConnectionType type) noexcept;

    // 启动监听可写事件
    void listen_write();

    // 启动监听可读事件
    void listen_read();

    void push_send_message(std::shared_ptr<AbstractProtocol> message,
                           std::function<void(std::shared_ptr<AbstractProtocol>)> done);

    void push_read_message(const std::string& req_id,
                           std::function<void(std::shared_ptr<AbstractProtocol>)> done);

    /**
     * @brief 得到本地地址
     *
     * @return std::shared_ptr<IPv4NetAddr> 对端地址
     */
    std::shared_ptr<IPv4NetAddr> get_local_addr();

    /**
     * @brief 得到对端地址
     *
     * @return std::shared_ptr<IPv4NetAddr> 对端地址
     */
    std::shared_ptr<IPv4NetAddr> get_peer_addr();

private:
    std::shared_ptr<IPv4NetAddr> m_local_addr { nullptr }; ///< 本地地址
    std::shared_ptr<IPv4NetAddr> m_peer_addr { nullptr };  ///< 对方服务器的地址

    std::shared_ptr<TcpBuffer> m_in_buffer { nullptr };  ///< 接收缓冲区
    std::shared_ptr<TcpBuffer> m_out_buffer { nullptr }; ///< 发送缓冲区

    std::shared_ptr<EventLoop> m_event_loop {
        nullptr
    }; ///< 当前指向的IO线程 当前主EventLoop函数

    std::shared_ptr<FdEvent> m_fd_event { nullptr }; ///< 这个是被动监听的那个，就是listen() 的到的m_fd_event

    std::shared_ptr<AbstractCoder> m_coder { nullptr }; ///< 编解码器

    TcpState m_state { TcpState::NotConnected }; ///< 连接状态

    int m_fd { -1 }; ///< 指向连接的套接字

    TcpConnectionType m_connection_type {
        TcpConnectionType::TcpConnectionByServer
    }; // 默认server类型

    std::vector<std::pair<std::shared_ptr<AbstractProtocol>,
                          std::function<void(std::shared_ptr<AbstractProtocol>)>>>
        m_write_dones;

    // AbstractProtocol::req_id 唯一的请求号
    // std::function: 回调函数
    // 使用map,方便找这个std::string的req_id
    std::map<std::string, std::function<void(std::shared_ptr<AbstractProtocol>)>>
        m_read_dones;

    // std::shared_ptr<RpcDispatcher> m_dispatcher;
};
} // namespace rpc
#endif