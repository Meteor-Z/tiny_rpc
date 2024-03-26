/**
 * @file tcp_acceptor.h
 * @author liuzechen (liuzechen.coder@qq.com)
 * @brief 维护的是 socket() -> bind() 这一过程
 * @version 0.1
 * @date 2024-03-15
 * @note
 * @copyright Copyright (c) 2024
 *
 *
 *
 *   TcpServr中的监听套接字
 *
 *  socket()            bind()            listen()         accept()
 *   创建一个监听套接字     绑定到一个端口上     变成监听套接字     一直进行监听
 */
#pragma once

#ifndef RPC_NET_TCP_TCP_ACCEPTOR_H
#define RPC_NET_TCP_TCP_ACCEPTOR_H

#include <google/protobuf/arenastring.h>
#include <memory>
#include <net/tcp/ipv4_net_addr.h>

namespace rpc {
/**
 * @brief 封装的是套接字中的accept这个过程
 *
 */
class TcpAcceptor {
public:
    TcpAcceptor(std::shared_ptr<IPv4NetAddr> local_addr);

    ~TcpAcceptor();

    TcpAcceptor(const TcpAcceptor&) = delete;
    TcpAcceptor(TcpAcceptor&&) = delete;
    TcpAcceptor& operator=(const TcpAcceptor&) = delete;
    TcpAcceptor& operator=(TcpAcceptor&&) = delete;

    /**
     * @brief 封装的是底层accept()函数，返回的是套接字和相关地址
     *
     * @return std::pair<int, std::shared_ptr<IPv4NetAddr>> pair <套接字，地址>
     */
    std::pair<int, std::shared_ptr<IPv4NetAddr>> accept();

    int get_listend_fd();

private:
    std::shared_ptr<IPv4NetAddr> m_local_addr; ///< 监听的本地地址
    int m_family { -1 };                       ///< 协议族
    int m_listenfd { -1 };                     ///< 监听的套接字
};
} // namespace rpc

#endif