#include <memory>
#include <sys/socket.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <fmt/core.h>
#include "common/log.h"
#include "net/tcp/ipv4_net_addr.h"
#include "net/tcp/tcp_acceptor.h"

namespace rpc {
TcpAcceptor::TcpAcceptor(std::shared_ptr<IPv4NetAddr> local_addr)
    : m_local_addr(local_addr) {
    if (!local_addr->check_valid()) {
        ERROR_LOG(fmt::format("invalid local addr {}", local_addr->to_string()));
        std::exit(2);
    }

    m_family = m_local_addr->get_family();

    m_listenfd = socket(m_family, SOCK_STREAM, 0);

    if (m_listenfd < 0) {
        ERROR_LOG(fmt::format("TcpAcceptor() init error, beacuse socket() error"));
        std::exit(2);
    }

    int valid = 0;

    // 设置成非阻塞，就是复用time_wait这一个状态的端口号
    int rt = setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEPORT, &valid, sizeof(valid));

    // 非必需
    if (rt == -1) {
        ERROR_LOG(
            fmt::format("setsockopt errno = {}, error = {}", errno, strerror(errno)));
    }

    socklen_t sock_len = m_local_addr->get_sock_len();
    sockaddr* sock_addr = m_local_addr->get_sock_addr();

    // 这里也是bind了，绑定到这个类型上。
    if (bind(m_listenfd, sock_addr, sock_len) != 0) {
        ERROR_LOG(
            fmt::format("bind() error  errno = {}, errno = {}", errno, strerror(errno)));
        std::exit(2);
    }

    // 服务端的listen, 变成被动监听
    if (listen(m_listenfd, 1000) != 0) {
        ERROR_LOG(fmt::format("listend() error, errno = {}, error = {}", errno,
                              strerror(errno)));
        std::exit(2);
    }
}

std::pair<int, std::shared_ptr<IPv4NetAddr>> TcpAcceptor::accept() {
    sockaddr_in client_addr {};

    socklen_t client_addr_len { sizeof(client_addr) };

    // 返回的是返回值，得到对应的文件描述符
    int client_fd =
        ::accept(m_listenfd, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_len);

    if (client_fd < 0) {
        ERROR_LOG(fmt::format("accept() error, errno = {}, error = {}", errno,
                              strerror(errno)));
        std::exit(2);
    }

    // IPv4NetAddr peer_addr(client_addr);
    // peer 是对方的客户端
    std::shared_ptr<IPv4NetAddr> peer_addr = std::make_shared<IPv4NetAddr>(client_addr);
    INFO_LOG(fmt::format("a client accept() success, addr = {}", peer_addr->to_string()));
    return std::make_pair(client_fd, peer_addr);
}

int TcpAcceptor::get_listend_fd() { return m_listenfd; }

TcpAcceptor::~TcpAcceptor() { INFO_LOG("~TcpAcceptor()"); }
} // namespace rpc