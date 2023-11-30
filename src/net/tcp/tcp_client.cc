#include <sys/epoll.h>
#include <unistd.h>
#include <cerrno>
#include <cmath>
#include <cstring>
#include <map>
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

    // fd_event的对象获取
    m_fd_event = FdEventGroup::Get_Fd_Event_Group()->get_fd_event(m_fd);

    // 设置非阻塞的
    m_fd_event->set_no_block();

    // 连接
    m_connection = std::make_shared<TcpConnection>(m_event_loop, m_fd, 128, peer_addr);

    // 设置成客户端的
    m_connection->set_connection_type(
        TcpConnection::TcpConnectionType::TcpConnectionByClient);
}

TcpClient::~TcpClient() {
    DEBUG_LOG("TcpClient:~TcpClient()");
    if (m_fd > 0) {
        close(m_fd);
    }
}
void TcpClient::write_message(
    std::shared_ptr<AbstractProtocol> message,
    std::function<void(std::shared_ptr<AbstractProtocol>)> done) {
     // client 将message 对象写入到Connection的buffer里面，done也要写入
    m_connection->push_send_message(message, done);

    // 启动connection的可写事件
    m_connection->listen_read();
}

void TcpClient::read_message(
    std::shared_ptr<AbstractProtocol> message,
    std::function<void(std::shared_ptr<AbstractProtocol>)> done) {}

void TcpClient::connect(std::function<void()> done) {
    // 系统的 connect 函数
    int result =
        ::connect(m_fd, m_peer_addr->get_sock_addr(), m_peer_addr->get_sock_len());

    if (result == 0) {
        INFO_LOG("connect success");

        // 执行回调函数
        if (done) {
            done();
        }
    } else if (result == -1) {
        if (errno == EINPROGRESS) {
            // epoll 监听可写事件,判断错误码
            m_fd_event->listen(FdEvent::TriggerEvent::OUT_EVENT, [this, done]() {
                int error = 0;
                socklen_t error_len = sizeof(error);

                /// TODO:什么东西
                getsockopt(m_fd, SOL_SOCKET, SO_ERROR, &error, &error_len);

                // 这里也算连接成功
                if (error == 0) {
                    DEBUG_LOG(
                        fmt::format("connect {} success", m_peer_addr->to_string()));
                    if (done) {
                        done();
                    }
                } else {
                    // 这里是其他错误，直接报错即可
                    ERROR_LOG(fmt::format(
                        "TcpClient connection() error, errnno = {}, error = {}", errno,
                        strerror(errno)));
                }

                // 去掉可写事件的监听
                m_fd_event->cancel(FdEvent::TriggerEvent::OUT_EVENT);
                m_event_loop->add_epoll_event(m_fd_event.get());
            });

            // 要加入到 epoll_event上面
            m_event_loop->add_epoll_event(m_fd_event.get());

            // 没有loop的时候才会进行loop
            if (!m_event_loop->is_looping()) {
                m_event_loop->loop();
            }
        }

    } else {
        ERROR_LOG(fmt::format("TcpClient connection() error, errnno = {}, error = {}",
                              errno, strerror(errno)));
    }

    if (!m_event_loop->is_looping()) {
        m_event_loop->loop();
    }
}
} // namespace rpc