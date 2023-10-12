#include "net/tcp/net_addr.hpp"
#include "common/log.hpp"
#include "net/tcp/tcp_buffer.hpp"

#include <cstring>
#include <fmt/core.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

namespace rpc {
    IPNetAddr::IPNetAddr(std::string_view ip, uint16_t port) : m_ip(ip), m_port(port) {
        std::memset(&m_addr, 0, sizeof(m_addr));
    
        m_addr.sin_family = AF_INET; // ipv4端口
        m_addr.sin_addr.s_addr = inet_addr(m_ip.c_str()); // ip地址
        m_addr.sin_port = htons(m_port);    
    }

    IPNetAddr::IPNetAddr(std::string_view addr) {
        size_t pos = addr.find_first_of(":");
        if (pos == addr.npos) {
            rpc::utils::ERROR_LOG(fmt::format("invalideu ipv4 addr {}", addr));
            return;
        }
        m_ip = addr.substr(0, pos);
        rpc::utils::DEBUG_LOG(fmt::format("IPNetAddr() initing... m_ip = {}", m_ip));

        std::string temp_port { addr.substr(pos + 1) };
        m_port = std::atoi(temp_port.c_str());
        rpc::utils::DEBUG_LOG(fmt::format("IPNetAddr() initing.... m_port = {}", m_port));

        std::memset(&m_addr, 0, sizeof(addr));
        m_addr.sin_family = AF_INET;
        m_addr.sin_addr.s_addr = inet_addr(m_ip.c_str());
        m_addr.sin_port = htons(m_port);
        
    }

    IPNetAddr::IPNetAddr(sockaddr_in addr): m_addr(addr) {
        m_ip = std::string(inet_ntoa(m_addr.sin_addr));
        m_port = ntohs(m_addr.sin_port);
    }

    sockaddr* IPNetAddr::get_sock_addr() { return reinterpret_cast<sockaddr*>(&m_addr); }

    socklen_t IPNetAddr::get_sock_len() { return sizeof(m_addr); }

    int IPNetAddr::get_family() { return AF_INET; }

    std::string IPNetAddr::to_string() { return std::string { fmt::format("{}:{}", m_ip, m_port)}; }
}