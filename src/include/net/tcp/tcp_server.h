/*
TcpServer:
通过epoll监听, listenfd的可读事件，当可读事件发生之后，就会调用accept函数获取clientfd
随机选出来一个subReactor,将clientfd读写事件注册到subReactor的epoll上，

>mainReactor只负责建立连接事件，不负责业务处理，也不关心已经连接套接字的IO事件
*/

#ifndef RPC_NET_TCP_TCP_SERVER_H
#define RPC_NET_TCP_TCP_SERVER_H

#include <memory>
#include "net/eventloop.h"
#include "net/fd_event/fd_event.h"
#include "net/io_thread/io_thread.h"
#include "net/io_thread/io_thread_group.h"
#include "net/tcp/ipv4_net_addr.h"
#include "net/tcp/tcp_acceptor.h"

namespace rpc {
class TcpServer {
public:
    // local:addr: 本地地址
    TcpServer(std::shared_ptr<IPv4NetAddr> local_addr);
    ~TcpServer();

    // 开启server端的EventLoop
    void start();

private:
    // 当有新客户端连接之后就会执行
    void on_accept();
    void init();

private:
    std::shared_ptr<TcpAcceptor> m_acceptor {};          // 监听套接字
    std::shared_ptr<IPv4NetAddr> m_local_addr {};        // 本地监听地址
    std::shared_ptr<EventLoop> m_main_event_loop {};     // mainReactor
    std::shared_ptr<IOThreadGroup> m_io_thread_group {}; // subReactor (子Reactor组)
    std::shared_ptr<FdEvent> m_listen_fd_event {};       // 监听的FdEvent
    int m_client_counts { 0 };                           // 连接的数量
};
} // namespace rpc

#endif