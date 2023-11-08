#include <functional>
#include "net/tcp/tcp_server.h"
#include "common/log.h"
#include "net/eventloop.h"
#include "net/fd_event/fd_event.h"
#include "net/io_thread/io_thread_group.h"
#include "net/tcp/tcp_acceptor.h"


namespace rpc {
    TcpServer::~TcpServer() { }

    TcpServer::TcpServer(std::shared_ptr<IPv4NetAddr> local_addr): m_local_addr(local_addr) {

        init();
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
        rpc::utils::INFO_LOG(fmt::format("tcpserver success get client, fd = {}", client_fd));
    }

    void TcpServer::init() {

        m_acceptor = std::make_shared<TcpAcceptor>(m_local_addr);
        m_main_event_loop = EventLoop::get_current_eventloop();
        m_io_thread_group = std::make_shared<IOThreadGroup>(2);
        rpc::utils::INFO_LOG(fmt::format("tcp server success on {}", m_local_addr->to_string()));

        m_listen_fd_event = std::make_shared<FdEvent>(m_acceptor->get_listend_fd());
        m_listen_fd_event->listen(FdEvent::TriggerEvent::IN_EVENT, std::bind(&TcpServer::on_accept, this));

        m_main_event_loop->add_epoll_event(m_listen_fd_event.get());
    }

   
    
}