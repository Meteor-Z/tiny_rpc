/**
TcpServr

socket()            bind()            listen()         accept()
创建一个监听套接字     绑定到一个端口上     变成监听套接字     一直进行监听 

*/

#ifndef RPC_NET_TCP_TCP_ACCEPTOR_H
#define RPC_NET_TCP_TCP_ACCEPTOR_H

#include <net/tcp/net_addr.hpp>

#include <memory>

namespace rpc {
    class TcpAcceptor {
    public:
        TcpAcceptor(std::shared_ptr<NetAddr> local_addr);
        ~TcpAcceptor();
        int accept();
    private:
        std::shared_ptr<NetAddr> m_local_addr;
        int m_family { -1 }; // 协议族
        int m_listenfd { -1 };
    };
}

#endif