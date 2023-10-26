#include <unistd.h>
#include "net/tcp/tcp_connection.h"
#include "common/log.hpp"
#include "net/fd_event.hpp"
#include "net/fd_event/fd_event_group.h"
#include "net/tcp/tcp_buffer.hpp"

namespace rpc {
TcpConnection::TcpConnection(std::shared_ptr<IOThread> io_thread, int fd,
                             int buffer_size,
                             std::shared_ptr<NetAddr> peer_addr)
    : m_io_thread(io_thread), m_peer_addr(peer_addr),
      m_state(TcpState::NotConnected) {

    m_in_buffer = std::make_shared<TcpBuffer>(buffer_size);
    m_out_buffer = std::make_shared<TcpBuffer>(buffer_size);

    m_fd_event = FdEventGroup::Get_Fd_Event_Group()->get_fd_event(fd);
    m_fd_event->listen(Fd_Event::TriggerEvent::IN_EVENT,
                       std::bind(&TcpConnection::read, this));
}

TcpConnection::~TcpConnection() {}

void TcpConnection::read() {
    // 1. 从socket 缓冲区调用read()函数读取in_buffer里面的东西

    if (m_state != TcpState::Connected) {
        rpc::utils::INFO_LOG(fmt::format(
            "client has already disconnected, addr = {}, clientfd {}",
            m_peer_addr->to_string(), m_fd_event->get_fd()));
    }

    // 是否读完？
    bool is_read_all{false};
    while (!is_read_all) {
        int read_count = m_in_buffer->can_write_bytes_num();
        int write_index = m_in_buffer->wtite_index();

        int rt = read(m_fd_event->get_fd(), &(m_in_buffer->m_buffer[write_index]), read_count);
        
        if (rt > 0) {
            m_in_buffer->adjust_write_index(read_count);
        }
    }
}

void TcpConnection::excute() {}

void TcpConnection::write() {}
} // namespace rpc