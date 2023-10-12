#ifndef RPC_NET_TCP_NET_ADDR_H
#define RPC_NET_TCP_NET_ADDR_H

#include <arpa/inet.h>
#include <cstdint>
#include <netinet/in.h>

#include <memory>
#include <string_view>

namespace rpc {
    class NetAddr {
    public:
        virtual std::shared_ptr<sockaddr> get_sock_addr() = 0;
        virtual socklen_t get_sock_len() = 0;
        virtual int set_family() = 0;
        virtual std::string addr_to_string() = 0;
        
    private:
    };

    class IPNetAddr: public NetAddr {
    public:
        IPNetAddr(std::string_view ip, uint16_t port);
        IPNetAddr(std::string_view addr);
        IPNetAddr(sockaddr_in addr);
        sockaddr* get_sock_addr();
        socklen_t get_sock_len();
        int get_family();
        std::string to_string();
    private:
        std::string m_ip { };
        uint16_t m_port { 0 };
        sockaddr_in m_addr;
    };
}

#endif