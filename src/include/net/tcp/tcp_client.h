/*
TcpClient

connect() -> write() -> read()

connect(): 连接对端机器
write(): 将rpc相应发送给客户端
read(): 读取客户端发来的请求，组成rpc请求

此连接是非堵塞的
返回0: 表示连接成功
返回-1: 但是errno = EINPROGRESS,
表示正在连接，可以添加到epoll中监听可写事件，等待就绪之后，调用getsockopt获取fd上的错误，
错误为0表示连接成功
其他error就是直接报错了。 并且两者都要去掉可写事件监听。
*/
#ifndef RPC_NET_TCP_TCP_CLIENT_H
#define RPC_NET_TCP_TCP_CLIENT_H

#include <functional>
#include <memory>
#include "net/fd_event/fd_event.h"
#include "net/coder/abstract_protocol.h"
#include "net/tcp/ipv4_net_addr.h"
#include "net/eventloop.h"
#include "net/tcp/tcp_connection.h"
#include "net/time/time_event.h"

namespace rpc {

#define NEW_MESSAGE(XX) \


class TcpClient {
public:
    TcpClient(std::shared_ptr<IPv4NetAddr> peer_addr);

    ~TcpClient();

    // 异步进行连接
    // 如果异步成功，那么 done会被执行
    void connect(std::function<void()> done);

    // write message (使用协议进行封装)
    // 如果成功，会调用 done函数，函数入参就是message
    void write_message(std::shared_ptr<AbstractProtocol> message,
                       std::function<void(std::shared_ptr<AbstractProtocol>)> done);

    // read message (使用协议进行封装)
    // 如果成功，会调用 done函数，函数入参就是message
    // req_id： 是读这个请求参数
    /**
     * @brief 发送请求
     *
     * @param req_id 请求号
     * @param done 执行的回调函数
     */
    void read_message(const std::string& req_id, std::function<void(std::shared_ptr<AbstractProtocol>)> done);

    /**
     * @brief 客户端能够停止下来，不要一直循环
     *
     */
    void stop();

    /**
     * @brief Get the peer addr object
     *
     * @return std::shared_ptr<IPv4NetAddr>
     */
    std::shared_ptr<IPv4NetAddr> get_peer_addr();

    /**
     * @brief Get the local addr object
     *
     * @return std::shared_ptr<IPv4NetAddr>
     */
    std::shared_ptr<IPv4NetAddr> get_local_addr();

    /**
     * @brief Get the connect error code object
     *
     * @return int 错误码
     */
    int get_connect_error_code();

    /**
     * @brief Get the connect error info object
     *
     * @return std::string 错误信息
     */
    std::string get_connect_error_info();

    /**
     * @brief 初始化本地地址
     *
     */
    void init_local_addr();

    /**
     * @brief 添加事件
     *
     * @param timer_event
     */
    void add_timer_event(std::shared_ptr<TimerEvent> timer_event);


private:
    std::shared_ptr<IPv4NetAddr> m_local_addr { nullptr };   ///< 本地地址
    std::shared_ptr<IPv4NetAddr> m_peer_addr { nullptr };    ///< 对端地址
    std::shared_ptr<EventLoop> m_event_loop { nullptr };     ///< 处理事件
    int m_fd { -1 };                                         ///< 文件描述符
    std::shared_ptr<FdEvent> m_fd_event { nullptr };         ///< 事件描述符
    std::shared_ptr<TcpConnection> m_connection { nullptr }; ///< 处理连接

    int m_connect_error_code { 0 };      ///< 连接对端的错误码
    std::string m_connect_error_info {}; ///< 错误信息
};
} // namespace rpc

#endif