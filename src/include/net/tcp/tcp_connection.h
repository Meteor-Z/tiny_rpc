#ifndef RPC_NET_TCP_TCP_CONNECTION_H
#define RPC_NET_TCP_TCP_CONNECTION_H

#include "net/fd_event.hpp"
#include "net/io_thread/io_thread.hpp"
#include "net/tcp/net_addr.hpp"
#include "net/tcp/tcp_buffer.hpp"
#include <cstddef>

namespace rpc {
class TcpConnection {
public:
    enum class TcpState {
        NotConnected = 1,
        Connected = 2,
        HalfClosing = 3, // 半连接
        Closed = 4,
    };

public:
    TcpConnection(std::shared_ptr<IOThread> io_thread, int fd, int buffer_size,
                  std::shared_ptr<NetAddr> peer_addr);
    ~TcpConnection();
    void on_read();
    void excute();
    void write();

private:
    std::shared_ptr<NetAddr> m_local_addr;
    std::shared_ptr<NetAddr> m_peer_addr;
    std::shared_ptr<TcpBuffer> m_in_buffer;         // 接收缓冲区
    std::shared_ptr<TcpBuffer> m_out_buffer;        // 发送缓冲区
    std::shared_ptr<IOThread> m_io_thread{nullptr}; // 线程
    std::shared_ptr<Fd_Event> m_fd_event{nullptr};  // 当前接听的事件
    TcpState m_state;                               // 当前状态
};
} // namespace rpc
#endif