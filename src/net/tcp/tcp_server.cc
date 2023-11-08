#include <functional>
#include "net/tcp/tcp_server.h"
#include "common/log.h"
#include "net/eventloop.h"
#include "net/fd_event/fd_event.h"
#include "net/io_thread/io_thread_group.h"
#include "net/tcp/tcp_acceptor.h"

namespace rpc {
TcpServer::~TcpServer() {}

TcpServer::TcpServer(std::shared_ptr<IPv4NetAddr> local_addr) : m_local_addr(local_addr) {
    // TODO():这里要有修改
    init();

    rpc::utils::INFO_LOG(
        fmt::format("tcp server success on {}", m_local_addr->to_string()));
}

void TcpServer::start() {
    // 开启循环
    m_io_thread_group->start();
    m_main_event_loop->loop();
}

// TODO: 将 clientfd加入到io线程中;
void TcpServer::on_accept() {
    int client_fd = m_acceptor->accept();
    ++m_client_counts;
    rpc::utils::INFO_LOG(fmt::format("tcp_server success get client, fd = {}", client_fd));

    // TODO:
    // m_io_thread_group->get_io_thread()->get_eventloop()->add_epoll_event();
}

void TcpServer::init() {
    m_acceptor = std::make_shared<TcpAcceptor>(m_local_addr);

    // 主线程的MainReactor 就是主线程的EventLoop
    m_main_event_loop = EventLoop::Get_Current_Eventloop();

    // io线程组
    m_io_thread_group = std::make_shared<IOThreadGroup>(4);

    m_listen_fd_event = std::make_shared<FdEvent>(m_acceptor->get_listend_fd());
    m_listen_fd_event->listen(FdEvent::TriggerEvent::IN_EVENT,
                              std::bind(&TcpServer::on_accept, this));

    // 加入到主Reactor里面
    m_main_event_loop->add_epoll_event(m_listen_fd_event.get());
}

} // namespace rpc