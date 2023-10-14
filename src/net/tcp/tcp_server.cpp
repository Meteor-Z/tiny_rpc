#include "net/tcp/tcp_server.hpp"
#include "common/log.hpp"
#include "net/eventloop.hpp"
#include "net/fd_event.hpp"
#include "net/io_thread/io_thread_group.hpp"
#include "net/tcp/tcp_acceptor.hpp"

#include <functional>

namespace rpc {
    TcpServer::~TcpServer() { }

    TcpServer::TcpServer(std::shared_ptr<NetAddr> local_addr): m_local_addr(local_addr) {
        std::cout << "2 -----" << std::endl;
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
        // m_io_thread_group->get_io_thread()->get_eventloop()->add_epoll_event(client_fd);
        rpc::utils::INFO_LOG(fmt::format("tcpserver success get client, fd = {}", client_fd));
    }
    void TcpServer::init() {
        std::cout << "3 -----" << std::endl;
        std::cout << "4 " + m_local_addr->to_string() << std::endl;
        m_acceptor = std::make_shared<TcpAcceptor>(m_local_addr);
        std::cout << "5 -----" << std::endl;
        // 这里寄了
        m_main_event_loop = EventLoop::get_current_eventloop();
        m_io_thread_group = std::make_shared<IOThreadGroup>(2);
        
        rpc::utils::ERROR_LOG("--------");

        rpc::utils::INFO_LOG(fmt::format("tcp server success on {}", m_local_addr->to_string()));

        m_listen_fd_event = std::make_shared<Fd_Event>(m_acceptor->get_listend_fd());
        m_listen_fd_event->listen(Fd_Event::TriggerEvent::IN_EVENT, std::bind(&TcpServer::on_accept, this));

        m_main_event_loop->add_epoll_event(m_listen_fd_event.get());
        
    }

   
    
}