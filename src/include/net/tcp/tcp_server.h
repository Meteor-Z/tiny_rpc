#ifndef RPC_NET_TCP_TCP_SERVER_H
#define RPC_NET_TCP_TCP_SERVER_H

#include <memory>
#include "net/eventloop.h"
#include "net/fd_event/fd_event.h"
#include "net/io_thread/io_thread.h"
#include "net/io_thread/io_thread_group.h"
#include "net/tcp/net_addr.h"
#include "net/tcp/tcp_acceptor.h"

namespace rpc {
class TcpServer {
public:
    TcpServer(std::shared_ptr<IPv4NetAddr> local_addr);
    ~TcpServer();
    void start();

private:
    // 当有客户端进行连接的时候就会执行，
    void on_accept();
    void init();

private:
    std::shared_ptr<TcpAcceptor> m_acceptor {};
    std::shared_ptr<IPv4NetAddr> m_local_addr {};            // 本地监听的地址
    std::shared_ptr<EventLoop> m_main_event_loop {};     // mainReactor
    std::shared_ptr<IOThreadGroup> m_io_thread_group {}; // subReactor (子Reactor组)
    std::shared_ptr<FdEvent> m_listen_fd_event {};
    int m_client_counts { 0 };
};
} // namespace rpc

#endif