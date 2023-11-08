#include <cstring>
#include <fmt/core.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include "net/tcp/net_addr.h"
#include "common/log.h"
#include "net/tcp/tcp_buffer.h"

namespace rpc {
    IPv4NetAddr::IPv4NetAddr(std::string_view ip, uint16_t port) : m_ip(ip), m_port(port) {
        std::memset(&m_addr, 0, sizeof(m_addr));
    
        m_addr.sin_family = AF_INET; // ipv4端口
        m_addr.sin_addr.s_addr = inet_addr(m_ip.c_str()); // ip地址
        m_addr.sin_port = htons(m_port);    
        rpc::utils::DEBUG_LOG("IPNetAddr init success.");
    }

    IPv4NetAddr::IPv4NetAddr(std::string_view addr) {
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
        rpc::utils::DEBUG_LOG("IPNetAddr init success.");
    }

    IPv4NetAddr::IPv4NetAddr(sockaddr_in addr): m_addr(addr) {
        m_ip = std::string(inet_ntoa(m_addr.sin_addr));
        m_port = ntohs(m_addr.sin_port);
        rpc::utils::DEBUG_LOG("IPNetAddr init success.");
    }

    bool IPv4NetAddr::check_valid() {
        if (m_ip.empty() || m_port == 0) { 
            std::cout << "寄了" << std::endl;
            return false; 
        }
        
        // 转换失败
        if (inet_addr(m_ip.c_str()) == INADDR_NONE) {
            std::cout << "这寄了" << std::endl;
            return false;
        }

        return true;
        
    }
    
    sockaddr* IPv4NetAddr::get_sock_addr() { return reinterpret_cast<sockaddr*>(&m_addr); }

    socklen_t IPv4NetAddr::get_sock_len() { return sizeof(m_addr); }

    int IPv4NetAddr::get_family() { return AF_INET; }

    std::string IPv4NetAddr::to_string() { return std::string { fmt::format("{}:{}", m_ip, m_port)}; }
} // namespace rpc