#include <sys/socket.h>
#include <fcntl.h>
#include <system_error>
#include <cerrno>
#include <cstring>
#include <fmt/core.h>
#include "common/log.h"
#include "net/tcp/net_addr.h"
#include <net/tcp/tcp_acceptor.h>

namespace rpc {
    TcpAcceptor::TcpAcceptor(std::shared_ptr<IPv4NetAddr> local_addr): m_local_addr(local_addr) {
        if (!local_addr->check_valid()) {
            // std::cout <<local_addr->to_string() << std::endl;
            rpc::utils::ERROR_LOG(fmt::format("invalid local addr {}", local_addr->to_string()));
            std::exit(0);
        }        

        m_family = m_local_addr->get_family();

        m_listenfd = socket(m_family, SOCK_STREAM, 0);
        
        if (m_listenfd < 0) {
            rpc::utils::ERROR_LOG(fmt::format("TcpAcceptor() init error, beacuse socket() error"));
            std::exit(0);
        }
        
        int valid = 0;

        // 允许服用这个端口 ？ todo!:这里干什么
        int rt = setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEPORT, &valid, sizeof(valid));
        if (rt != 0) {
            rpc::utils::ERROR_LOG(fmt::format("setsockopt errno = {}, error = {}", errno, strerror(errno)));
        }

        socklen_t sock_len = m_local_addr->get_sock_len();
        sockaddr* sockaddr_ptr = m_local_addr->get_sock_addr();
        if (bind(m_listenfd, sockaddr_ptr, sock_len) != 0) {
            rpc::utils::ERROR_LOG(fmt::format("bind() error  errno = {}, errno = {}", errno, strerror(errno)));
            std::exit(0);
        }
        
        if (listen(m_listenfd, 1000) != 0) {
            rpc::utils::ERROR_LOG(fmt::format("listend() error, errno = {}, error = {}", errno, strerror(errno)));
            std::exit(0);
        }
    }
    
    // 只实现了 ipv4协议
    int TcpAcceptor::accept() {
        if (m_family == AF_INET) {
            sockaddr_in client_addr;
            std::memset(&client_addr, 0, sizeof(client_addr));
            
            socklen_t client_addr_len { sizeof(client_addr) };

            int client_fd = ::accept(m_listenfd, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_len);
            if (client_fd < 0) {
                rpc::utils::ERROR_LOG(fmt::format("accept() error, errno = {}, error = {}", errno, strerror(errno)));
                std::exit(0);
            }

            IPv4NetAddr peer_addr(client_addr);
            rpc::utils::INFO_LOG(fmt::format("a client accept() success, addr = {}",peer_addr.to_string()));
            return client_fd;
        } else {
            return false;
        }
        
    }

    int TcpAcceptor::get_listend_fd() {
        return m_listenfd; 
    }

    TcpAcceptor::~TcpAcceptor() {
        
    }
}