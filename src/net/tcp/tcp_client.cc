#include <asm-generic/socket.h>
#include <cerrno>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/socket.h>
#include "net/fd_event/fd_event.h"
#include "net/fd_event/fd_event_group.h"
#include "net/tcp/tcp_client.h"
#include "net/eventloop.h"
#include "net/tcp/tcp_connection.h"
#include "common/log.h"

namespace rpc {
TcpClient::TcpClient(std::shared_ptr<IPv4NetAddr> peer_addr) : m_peer_addr(peer_addr) {
    m_event_loop = EventLoop::Get_Current_Eventloop();
    m_fd = socket(peer_addr->get_family(), SOCK_STREAM, 0);

    // if (m_fd < 0) {
    //
    // }

    m_fd_event = FdEventGroup::Get_Fd_Event_Group()->get_fd_event(m_fd);
    m_connection = std::make_shared<TcpConnection>(m_event_loop, m_fd, 128, peer_addr);

    // 设置成客户端的
    m_connection->set_connection_type(
        TcpConnection::TcpConnectionType::TcpConnectionByClient);
}

TcpClient::~TcpClient() {
    if (m_fd > 0) {
        close(m_fd);
    }
}
void TcpClient::write_message(
    std::shared_ptr<AbstractProtocol> message,
    std::function<void(std::shared_ptr<AbstractProtocol>)> done) {}

void TcpClient::read_message(
    std::shared_ptr<AbstractProtocol> message,
    std::function<void(std::shared_ptr<AbstractProtocol>)> done) {}

void TcpClient::connect(std::function<void()> done) {
    // 系统的 connect 函数
    int result =
        ::connect(m_fd, m_peer_addr->get_sock_addr(), m_peer_addr->get_sock_len());

    if (result == 0) {
        rpc::utils::INFO_LOG("connect success");
        // 执行回调函数
        if (done) {
            done();
        }
    } else if (result == -1 && errno == EINPROGRESS) {
        // epoll 监听可写事件,判断错误码
        m_fd_event->listen(FdEvent::TriggerEvent::OUT_EVENT, [this, done]() {
            int error = 0;
            socklen_t error_len = sizeof(error);

            // TODO:什么东西
            getsockopt(m_fd, SOL_SOCKET, SO_ERROR, &error, &error_len);

            if (error == 0) {
                rpc::utils::DEBUG_LOG(
                    fmt::format("connect {} success", m_peer_addr->to_string()));
                if (done) {
                    done();
                }
            } else {
                rpc::utils::ERROR_LOG(
                    fmt::format("TcpClient connection() error, errnno = {}, error = {}",
                                errno, strerror(errno)));
            }
        });
    } else {
        rpc::utils::ERROR_LOG(
            fmt::format("TcpClient connection() error, errnno = {}, error = {}", errno,
                        strerror(errno)));
    }
}
} // namespace rpc