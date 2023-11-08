/**
TcpServr中的监听套接字

socket()            bind()            listen()         accept()
创建一个监听套接字     绑定到一个端口上     变成监听套接字     一直进行监听

*/

#ifndef RPC_NET_TCP_TCP_ACCEPTOR_H
#define RPC_NET_TCP_TCP_ACCEPTOR_H

#include <memory>
#include <net/tcp/net_addr.h>

namespace rpc {
class TcpAcceptor {
public:
    TcpAcceptor(std::shared_ptr<IPv4NetAddr> local_addr);
    ~TcpAcceptor();
    int accept();
    int get_listend_fd();

private:
    std::shared_ptr<IPv4NetAddr> m_local_addr; // 客户端的地址 bind的地址
    int m_family { -1 };                   // 协议族
    int m_listenfd { -1 };                 // 文件描述符
};
} // namespace rpc

#endif