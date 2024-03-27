#include <functional>
#include <thread>
#include <algorithm>
#include "net/tcp/ipv4_net_addr.h"
#include "net/tcp/tcp_connection.h"
#include "net/tcp/tcp_server.h"
#include "net/tcp/tcp_acceptor.h"
#include "net/fd_event/fd_event.h"
#include "net/io_thread/io_thread_group.h"
#include "net/eventloop.h"
#include "common/log.h"

namespace rpc {
TcpServer::~TcpServer() { DEBUG_LOG("~TcpServer()"); }

TcpServer::TcpServer(std::shared_ptr<IPv4NetAddr> local_addr) : m_local_addr(local_addr) {
    // accepter
    m_acceptor = std::make_shared<TcpAcceptor>(m_local_addr);

    // 主线程的MainReactor 就是主线程的EventLoop
    m_main_event_loop = EventLoop::Get_Current_Eventloop();

    // io线程组要开多大，和当前的的CPU数量的大小有关
    m_io_thread_group = std::make_shared<IOThreadGroup>(
        std::max(std::thread::hardware_concurrency() / 2, 4u));

    // 监听的客户端，有新连接的时候，就是可读事件
    m_listen_fd_event = std::make_shared<FdEvent>(m_acceptor->get_listend_fd());

    m_listen_fd_event->listen(FdEvent::TriggerEvent::IN_EVENT,
                              std::bind(&TcpServer::on_accept, this));

    // 将这个文件描述符加入到mainReactor里面，相当于处理请求了
    m_main_event_loop->add_epoll_event(m_listen_fd_event);

    INFO_LOG(fmt::format("tcp server success on {}", m_local_addr->to_string()));
}

void TcpServer::start() {
    // 开启循环
    m_io_thread_group->start();
    m_main_event_loop->loop();
}

void TcpServer::on_accept() {
    // int client_fd = m_acceptor->accept();

    // struct client {
    //     first: fd;          // 套接字
    //     second: IPv4NetAddr // 地址
    // };

    // 得到新连接 <文件描述符，地址>
    std::pair<int, std::shared_ptr<IPv4NetAddr>> client = m_acceptor->accept();

    int client_fd = client.first;

    // 对方地址
    std::shared_ptr<IPv4NetAddr> peer_addr = client.second;

    ++m_client_counts;

    // 在这里得到对应的某一个线程
    std::shared_ptr<IOThread> io_thread = m_io_thread_group->get_io_thread();

    //  构造
    std::shared_ptr<TcpConnection> connection = std::make_shared<TcpConnection>(
        io_thread->get_eventloop(), client_fd, 128, m_local_addr, peer_addr);

    // 设置已经连接
    connection->set_state(TcpConnection::TcpState::Connected);

    // 不会析构
    m_client.insert(connection);
    INFO_LOG(fmt::format("tcp_server success get client, fd = {}", client_fd));
}

} // namespace rpc