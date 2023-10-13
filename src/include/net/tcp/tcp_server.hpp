#ifndef RPC_NET_TCP_TCP_SERVER_H
#define RPC_NET_TCP_TCP_SERVER_H

#include "net/eventloop.hpp"
#include "net/fd_event.hpp"
#include "net/io_thread/io_thread.hpp"
#include "net/io_thread/io_thread_group.hpp"
#include "net/tcp/net_addr.hpp"
#include "net/tcp/tcp_acceptor.hpp"
#include <memory>

namespace rpc {
    class TcpServer {
    public:
        TcpServer(std::shared_ptr<NetAddr> local_addr);  
        ~TcpServer();
        void start();
    private:
        // 当有客户端进行连接的时候就会执行，
        void on_accept();
        void init();
    private:
        std::shared_ptr<TcpAcceptor> m_acceptor { };
        std::shared_ptr<NetAddr> m_local_addr { }; // 本地监听的地址
        std::shared_ptr<EventLoop> m_main_event_loop { }; // mainReactor
        std::shared_ptr<IOThreadGroup> m_io_thread_group { }; // subReactor (子Reactor组)
        std::shared_ptr<Fd_Event> m_listen_fd_event { };
        int m_client_counts { 0 };
    };
}

#endif