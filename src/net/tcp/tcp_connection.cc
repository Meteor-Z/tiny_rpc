#include <asm-generic/errno-base.h>
#include <cerrno>
#include <cmath>
#include <fmt/core.h>
#include <math.h>
#include <sys/socket.h>
#include <type_traits>
#include <unistd.h>
#include <vector>
#include <fmt/format.h>
#include "net/eventloop.h"
#include "net/tcp/ipv4_net_addr.h"
#include "net/tcp/tcp_connection.h"
#include "common/log.h"
#include "net/fd_event/fd_event.h"
#include "net/fd_event/fd_event_group.h"
#include "net/tcp/tcp_buffer.h"
#include "net/time/time_event.h"

namespace rpc {
TcpConnection::TcpConnection(std::shared_ptr<EventLoop> event_loop, int fd,
                             int buffer_size, std::shared_ptr<IPv4NetAddr> peer_addr)
    : m_event_loop(event_loop), m_peer_addr(peer_addr), m_state(TcpState::NotConnected),
      m_fd(fd) {

    // 初始化buffer的大小
    m_in_buffer = std::make_shared<TcpBuffer>(buffer_size);
    m_out_buffer = std::make_shared<TcpBuffer>(buffer_size);

    m_fd_event = FdEventGroup::Get_Fd_Event_Group()->get_fd_event(fd);

    // 设置称非堵塞
    m_fd_event->set_no_block();

    m_fd_event->listen(FdEvent::TriggerEvent::IN_EVENT,
                       std::bind(&TcpConnection::read, this));

    // TODO
    // m_event_loop->get_eventloop()->add_epoll_event(m_fd_event.get());
    m_event_loop->add_epoll_event(m_fd_event.get());
}

TcpConnection::~TcpConnection() {}

void TcpConnection::set_state(const TcpConnection::TcpState& state) { m_state = state; }

TcpConnection::TcpState TcpConnection::get_state() const noexcept { return m_state; }

void TcpConnection::read() {
    // 如果不是连接中
    if (m_state != TcpState::Connected) {
        INFO_LOG(
            fmt::format("client has already disconnected, addr = {}, clientfd {}",
                        m_peer_addr->to_string(), m_fd_event->get_fd()));
        return;
    }

    // 是否读完？
    bool is_read_all { false };
    bool is_close { false };

    while (!is_read_all) {
        //
        if (m_in_buffer->can_write_bytes_num() == 0) {
            fmt::println("hello");
            m_in_buffer->resize_buffer(2 * m_in_buffer->get_buffer().size());
        }
        int read_count = m_in_buffer->can_write_bytes_num();
        int write_index = m_in_buffer->wtite_index();

        int rt = ::read(m_fd, &(m_in_buffer->get_buffer()[write_index]), read_count);
        INFO_LOG(fmt::format("success read {} bytes fron {}, client fd = {}",
                                         rt, m_peer_addr->to_string(), m_fd));

        // 读成功了！进行调整
        if (rt > 0) {
            m_in_buffer->adjust_write_index(rt);

            // 还有数据没有读完
            if (rt == read_count) {
                continue;
            } else if (rt < read_count) {
                is_read_all = true;
                break;
            }

        } else if (rt == 0) {
            is_close = true;
            break;
        } else if (rt == -1 && errno == EAGAIN) {
            is_read_all = true;
            break;
        }
    }

    // 处理关闭连接
    if (is_close) {
        DEBUG_LOG(fmt::format("peer closed, peer addr = {}, client_fd = {}",
                                          m_peer_addr->to_string(), m_fd));
        clear();
        return;
    }

    if (!is_read_all) {
        INFO_LOG("not read all data");
    }
    // 读完就开始进行rpc解析
    excute();
}

// 将RPC请求执行业务逻辑，获取RPC相应，再将RPC响应发送回去
void TcpConnection::excute() {
    // 先将数据读取出来
    std::vector<char> temp;
    int size = m_in_buffer->can_read_bytes_num();
    temp.resize(size);
    m_in_buffer->read_from_buffer(temp, size);

    std::string message {};
    for (int i = 0; i < temp.size(); i++) {
        message += temp[i];
    }

    // 写入到 buffer 里面
    m_out_buffer->write_to_buffer(message.c_str(), message.size());
    m_fd_event->listen(FdEvent::TriggerEvent::OUT_EVENT,
                       std::bind(&TcpConnection::on_write, this));

    // 添加事件
    // m_io_thread->get_eventloop()->add_epoll_event(m_fd_event.get());

    m_event_loop->add_epoll_event(m_fd_event.get());
    INFO_LOG(
        fmt::format("success get request from client {}", m_peer_addr->to_string()));
}

void TcpConnection::shutdown() {
    if (m_state == TcpConnection::TcpState::Closed ||
        m_state == TcpConnection::TcpState::NotConnected) {
        return;
    }

    // 在半关闭
    m_state = TcpConnection::TcpState::HalfClosing;
    // 触发四次挥手 fin报文
    ::shutdown(m_fd, SHUT_RDWR);
}
void TcpConnection::clear() {
    if (m_state == TcpConnection::TcpState::Closed) {
        return;
    }

    // 取消监听读和写事件
    m_fd_event->cancel(FdEvent::TriggerEvent::IN_EVENT);
    m_fd_event->cancel(FdEvent::TriggerEvent::OUT_EVENT);

    // 去除套接字
    // m_io_thread->get_eventloop()->delete_epoll_event(m_fd_event.get());

    // 去除套接字
    m_event_loop->delete_epoll_event(m_fd_event.get());

    // 这时候才会正式关闭
    m_state = TcpConnection::TcpState::Closed;
}

void TcpConnection::set_connection_type(TcpConnectionType type) noexcept { m_connection_type = type; }
void TcpConnection::on_write() {
    // 将当前 out_buffer 发送到到 client

    // 如果已经关闭了
    if (m_state != TcpState::Connected) {
        ERROR_LOG(fmt::format(
            "on_write() error, already disconnected, addr = {}, client fd = {}",
            m_peer_addr->to_string(), m_fd));
        return;
    }

    bool is_write_all = false;
    // 一直发送，直到发送完
    while (true) {
        if (m_out_buffer->can_read_bytes_num() == 0) {
            DEBUG_LOG(fmt::format("no data need to send to client {}",
                                              m_peer_addr->to_string()));
            is_write_all = true;
            break;
        }
        int size = m_out_buffer->can_read_bytes_num();
        int read_index = m_out_buffer->read_index();

        // 发送
        int result = ::write(m_fd, &(m_out_buffer->get_buffer()[read_index]), size);

        // 发送完了
        if (result >= size) {
            DEBUG_LOG(
                fmt::format("no data need to client {}", m_peer_addr->to_string()));
            is_write_all = true;
            break;
        }
        // 缓冲区已经满了
        if (result == -1 && errno == EAGAIN) {
            ERROR_LOG(
                fmt::format("write data error, errno = EAGIN and result = -1"));
            break;
        }
    }

    if (is_write_all) {
        m_fd_event->cancel(FdEvent::TriggerEvent::OUT_EVENT);
        // m_io_thread->get_eventloop()->add_epoll_event(m_fd_event.get());
        m_event_loop->add_epoll_event(m_fd_event.get());
    }
}

} // namespace rpc