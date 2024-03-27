#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include "net/tcp/tcp_client.h"
#include "common/error_code.h"
#include "common/log.h"
#include "net/eventloop.h"
#include "net/fd_event/fd_event.h"
#include "net/fd_event/fd_event_group.h"
#include "net/tcp/ipv4_net_addr.h"
#include "net/tcp/tcp_connection.h"
#include "net/time/time_event.h"

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

    // 连接,并且设置成客户端的
    m_connection = std::make_shared<TcpConnection>(m_event_loop, m_fd, 128, nullptr, peer_addr,
                                                   TcpConnection::TcpConnectionType::TcpConnectionByClient);

    // 设置成客户端的
    // m_connection->set_connection_type(TcpConnection::TcpConnectionType::TcpConnectionByClient);
}

TcpClient::~TcpClient() {
    if (m_fd > 0) {
        close(m_fd);
    }
    DEBUG_LOG("TcpClient:~TcpClient()");
}

void TcpClient::write_message(std::shared_ptr<AbstractProtocol> message,
                              std::function<void(std::shared_ptr<AbstractProtocol>)> done) {
    // client 将message 对象写入到Connection的buffer里面，done也要写入
    m_connection->push_send_message(message, done);

    // 启动connection的可写事件
    m_connection->listen_write();
}

void TcpClient::read_message(const std::string& req_id, std::function<void(std::shared_ptr<AbstractProtocol>)> done) {
    m_connection->push_read_message(req_id, done);
    // 1. 监听可读事件
    m_connection->listen_read();

    // 2. 从buffer里面 decode得到message事件
}

void TcpClient::connect(std::function<void()> done) {
    // 系统的 connect 函数
    int rt = ::connect(m_fd, m_peer_addr->get_sock_addr(), m_peer_addr->get_sock_len());
    // 如果正常的话 
    if (rt == 0) {
        DEBUG_LOG(fmt::format("connect [%s] sussess", m_peer_addr->to_string().c_str()));
        m_connection->set_state(TcpConnection::TcpState::Connected);
        init_local_addr();
        if (done) {
            done();
        }
    } else if (rt == -1) {
        if (errno == EINPROGRESS) {
            // epoll 监听可写事件，然后判断错误码
            m_fd_event->listen(FdEvent::TriggerEvent::OUT_EVENT, [this, done]() {
                int rt = ::connect(m_fd, m_peer_addr->get_sock_addr(), m_peer_addr->get_sock_len());
                if ((rt < 0 && errno == EISCONN) || (rt == 0)) {
                    DEBUG_LOG(fmt::format("connect [%s] sussess", m_peer_addr->to_string()));
                    init_local_addr();
                    m_connection->set_state(TcpConnection::TcpState::Connected);
                } else {
                    if (errno == ECONNREFUSED) {
                        m_connect_error_code = ERROR_PEER_CLOSED;
                        m_connect_error_info = "connect refused, sys error = " + std::string(strerror(errno));
                    } else {
                        m_connect_error_code = ERROR_FAILED_CONNECT;
                        m_connect_error_info = "connect unkonwn error, sys error = " + std::string(strerror(errno));
                    }
                    ERROR_LOG(fmt::format("connect errror, errno= {}, error= {}", errno, strerror(errno)));
                    close(m_fd);
                    m_fd = socket(m_peer_addr->get_family(), SOCK_STREAM, 0);
                }

                // 连接完后需要去掉可写事件的监听，不然会一直触发
                m_event_loop->delete_epoll_event(m_fd_event);
                DEBUG_LOG("now begin to done");
                // 如果连接完成，才会执行回调函数
                if (done) {
                    done();
                }
            });
            m_event_loop->add_epoll_event(m_fd_event);

            if (!m_event_loop->is_looping()) {
                m_event_loop->loop();
            }
        } else {
            ERROR_LOG(fmt::format("connect errror, errno= {}, error= {}", errno, strerror(errno)));
            m_connect_error_code = ERROR_FAILED_CONNECT;
            m_connect_error_info = "connect error, sys error = " + std::string(strerror(errno));
            if (done) {
                done();
            }
        }
    }
}
void TcpClient::stop() {
    if (m_event_loop->is_looping()) {
        m_event_loop->stop();
    }
}

int TcpClient::get_connect_error_code() { return m_connect_error_code; }

std::string TcpClient::get_connect_error_info() { return m_connect_error_info; }

std::shared_ptr<IPv4NetAddr> TcpClient::get_peer_addr() { return m_peer_addr; }

std::shared_ptr<IPv4NetAddr> TcpClient::get_local_addr() { return m_local_addr; };

void TcpClient::init_local_addr() {
    sockaddr_in local_addr {};
    socklen_t len { sizeof(local_addr) };

    int ret = getsockname(m_fd, reinterpret_cast<sockaddr*>(&local_addr), &len);
    if (ret != 0) {
        ERROR_LOG(fmt::format("initaddr error, getsockname error, errno = {}", strerror(errno)));
        return;
    }

    m_local_addr = std::make_shared<IPv4NetAddr>(local_addr);
}
void TcpClient::add_timer_event(std::shared_ptr<TimerEvent> timer_event) { m_event_loop->add_timer_event(timer_event); }

} // namespace rpc