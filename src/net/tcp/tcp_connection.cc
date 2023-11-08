#include <unistd.h>
#include <vector>
#include <fmt/format.h>
#include "net/tcp/ipv4_net_addr.h"
#include "net/tcp/tcp_connection.h"
#include "common/log.h"
#include "net/fd_event/fd_event.h"
#include "net/fd_event/fd_event_group.h"
#include "net/tcp/tcp_buffer.h"

namespace rpc {
TcpConnection::TcpConnection(std::shared_ptr<IOThread> io_thread, int fd, int buffer_size,
                             std::shared_ptr<IPv4NetAddr> peer_addr)
    : m_io_thread(io_thread), m_peer_addr(peer_addr), m_state(TcpState::NotConnected),
      m_fd(fd) {

    // 初始化buffer的大小
    m_in_buffer = std::make_shared<TcpBuffer>(buffer_size);
    m_out_buffer = std::make_shared<TcpBuffer>(buffer_size);

    m_fd_event = FdEventGroup::Get_Fd_Event_Group()->get_fd_event(fd);
    m_fd_event->listen(FdEvent::TriggerEvent::IN_EVENT,
                       std::bind(&TcpConnection::read, this));
}

TcpConnection::~TcpConnection() {}

void TcpConnection::read() {
    // 如果不是连接中
    if (m_state != TcpState::Connected) {
        rpc::utils::INFO_LOG(
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
            m_in_buffer->resize_buffer(2 * m_in_buffer->get_buffer().size());
        }
        int read_count = m_in_buffer->can_write_bytes_num();
        int write_index = m_in_buffer->wtite_index();

        int rt = ::read(m_fd_event->get_fd(), &(m_in_buffer->get_buffer()[write_index]),
                        read_count);
        rpc::utils::INFO_LOG(fmt::format("success read {} bytes fron {}, client fd = {}",
                                         rt, m_peer_addr->to_string(), m_fd));
        // 读成功了！进行调整
        if (rt > 0) {
            m_in_buffer->adjust_write_index(read_count);

            // 还有数据没有读完
            if (rt == read_count) {
                continue;
            } else if (rt < read_count) {
                is_read_all = true;
                break;
            }

        } else {
            is_close = true;
        }
    }

    // 处理关闭连接
    if (is_close) {
        rpc::utils::DEBUG_LOG(fmt::format("peer closed, peer addr = {}, client_fd = {}",
                                          m_peer_addr->to_string(), m_fd));
    }

    if (!is_read_all) {
        rpc::utils::INFO_LOG("not read all data");
    }
    // 读完就开始进行rpc解析
    excute();
}

// 将RPC请求执行业务逻辑，获取RPC相应，再将RPC响应发送回去
void TcpConnection::excute() {
    std::vector<char> temp;
    int size = m_in_buffer->can_read_bytes_num();
    temp.resize(size);
    m_in_buffer->read_from_buffer(temp, size);

    rpc::utils::INFO_LOG(fmt::format("success get request from client {}", m_peer_addr->to_string()));

    
    
}

void TcpConnection::write() {}
} // namespace rpc