/*
TcpClient

connect() -> write() -> read()

connect(): 连接对端机器
write(): 将rpc相应发送给客户端
read(): 读取客户端发来的请求，组成rpc请求

此连接是非堵塞的
返回0: 表示连接成功
返回-1: 但是errno = EINPROGRESS, 表示正在连接，可以添加到epoll中监听可写事件，等待就绪之后，调用getsockopt获取fd上的错误，
错误为0表示连接成功
其他error就是直接报错了。 并且两者都要去掉可写事件监听。
*/
#ifndef RPC_NET_TCP_TCP_CLIENT_H
#define RPC_NET_TCP_TCP_CLIENT_H

#include <functional>
#include <memory>
#include "net/fd_event/fd_event.h"
#include "net/tcp/abstract_protocol.h"
#include "net/tcp/ipv4_net_addr.h"
#include "net/eventloop.h"
#include "net/tcp/tcp_connection.h"

namespace rpc {
class TcpClient {
public:
    TcpClient(std::shared_ptr<IPv4NetAddr> peer_addr);

    ~TcpClient();

    // 异步进行连接
    // 如果异步成功，那么 done会被执行
    void connect(std::function<void()> done);

    // write message (使用协议进行封装)
    // 如果成功，会调用 done函数，函数入参就是message
    void write_message(std::shared_ptr<AbstractProtocol> message,
                       std::function<void(std::shared_ptr<AbstractProtocol>)> done);

    // read message (使用协议进行封装)
    // 如果成功，会调用 done函数，函数入参就是message
    void read_message(std::shared_ptr<AbstractProtocol> message,
                      std::function<void(std::shared_ptr<AbstractProtocol>)> done);

private:
    std::shared_ptr<IPv4NetAddr> m_peer_addr { nullptr };    // 对端地址
    std::shared_ptr<EventLoop> m_event_loop { nullptr };     // 处理事件
    int m_fd { -1 };                                         // 文件描述符
    std::shared_ptr<FdEvent> m_fd_event { nullptr };         // 事件
    std::shared_ptr<TcpConnection> m_connection { nullptr }; // 处理连接
};
} // namespace rpc

#endif