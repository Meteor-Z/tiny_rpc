/*
TcpConnection()

read() -> excute() -> write()

read(): 读取客户端发来的请求，然后组装成RPC请求
excute(): 将RPC请求作为入参，执行业务逻辑得到RPC相应
write(): 将RPC相应发送到客户端

以上三个步骤一直执行。
*/

#ifndef RPC_NET_TCP_TCP_CONNECTION_H
#define RPC_NET_TCP_TCP_CONNECTION_H

#include <cstddef>
#include "net/fd_event/fd_event.h"
#include "net/io_thread/io_thread.h"
#include "net/tcp/ipv4_net_addr.h"
#include "net/tcp/tcp_buffer.h"

namespace rpc {
class TcpConnection {
public:
    // 连接状态
    enum class TcpState {
        NotConnected = 1, // 无连接
        Connected = 2,    // 连接
        HalfClosing = 3,  // 半连接
        Closed = 4,       // 关闭
    };

public:
    // io_thread: 当前io线程
    // fd: 指向的那个套接字
    // buffer_size: 初始化buffer的大小
    // peer_addr: 地址
    TcpConnection(std::shared_ptr<IOThread> io_thread, int fd, int buffer_size,
                  std::shared_ptr<IPv4NetAddr> peer_addr);

    ~TcpConnection();

    // 可读时间发生之后就会执行这个函数
    void read();

    // excute进行
    void excute();

    // 回调函数
    void on_write();

    void set_state(const TcpConnection::TcpState& state);

    TcpConnection::TcpState get_state();

    // 清除连接
    void clear();

    // 主动关闭
    void shutdown();

private:
    std::shared_ptr<IPv4NetAddr> m_local_addr { nullptr }; // 本地地址
    std::shared_ptr<IPv4NetAddr> m_peer_addr { nullptr };  // 对方服务器的地址
    std::shared_ptr<TcpBuffer> m_in_buffer { nullptr };    // 接收缓冲区
    std::shared_ptr<TcpBuffer> m_out_buffer { nullptr };   // 发送缓冲区
    std::shared_ptr<IOThread> m_io_thread { nullptr };     // 当前指向的IO线程
    std::shared_ptr<FdEvent> m_fd_event { nullptr };       // 监听的文件描述符
    TcpState m_state { TcpState::NotConnected };           // 连接状态
    int m_fd { -1 };                                       // 指向的套接字
};
} // namespace rpc
#endif