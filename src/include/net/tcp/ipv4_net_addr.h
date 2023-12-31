/*
对于IPv4进行封装
此处不考虑其他ip地址
*/
#ifndef RPC_NET_TCP_IPV4_NET_ADDR_H
#define RPC_NET_TCP_IPV4_NET_ADDR_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdint>
#include <memory>
#include <string_view>
#include <string>

/*
IP通用套接字接口 NetAddr
*/
namespace rpc {
// class NetAddr {
// public:
//     virtual sockaddr* get_sock_addr() = 0;
//     virtual socklen_t get_sock_len() = 0;

//     // 得到协议镞
//     virtual int get_family() = 0;

//     virtual std::string to_string() = 0;
//     virtual bool check_valid() = 0;
// };

// IPv4套接字
class IPv4NetAddr {
public:
    // 构造函数

    IPv4NetAddr(std::string_view ip, uint16_t port);
    IPv4NetAddr(std::string_view addr);
    IPv4NetAddr(sockaddr_in addr);

    sockaddr* get_sock_addr() noexcept;

    socklen_t get_sock_len() const noexcept;

    // 得到协议镞
    int get_family() const noexcept;

    // to format
    std::string to_string() const noexcept;

    bool check_valid();

private:
    std::string m_ip {};   // ip地址
    uint16_t m_port { 0 }; // 端口号
    sockaddr_in m_addr {}; // c style 的 sockaddr_in 就是对整个进行封装
};
} // namespace rpc

#endif