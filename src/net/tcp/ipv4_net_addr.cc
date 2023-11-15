#include <cstring>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <fmt/core.h>
#include "net/tcp/ipv4_net_addr.h"
#include "common/log.h"
#include "net/tcp/tcp_buffer.h"

namespace rpc {
IPv4NetAddr::IPv4NetAddr(std::string_view ip, uint16_t port) : m_ip(ip), m_port(port) {
    std::memset(&m_addr, 0, sizeof(m_addr));

    // 协议镞 地址 端口 设置
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(m_ip.c_str());
    m_addr.sin_port = htons(m_port);

    rpc::utils::DEBUG_LOG("IPv4NetAddr init success.");
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

IPv4NetAddr::IPv4NetAddr(sockaddr_in addr) : m_addr(addr) {
    m_ip = std::string(inet_ntoa(m_addr.sin_addr));
    m_port = ntohs(m_addr.sin_port);
    rpc::utils::DEBUG_LOG("IPNetAddr init success.");
}

bool IPv4NetAddr::check_valid() {
    if (m_ip.empty()) {
        rpc::utils::DEBUG_LOG(fmt::format("ip is empty(), error"));
        return false;
    }

    if (m_port < 0 || m_port > 65535) {
        rpc::utils::DEBUG_LOG(fmt::format("m_port is error"));
        return false;
    }

    // 转换失败
    if (inet_addr(m_ip.c_str()) == INADDR_NONE) {
        rpc::utils::DEBUG_LOG(fmt::format("can not to transform"));
        return false;
    }

    return true;
}

sockaddr* IPv4NetAddr::get_sock_addr() noexcept {
    return reinterpret_cast<sockaddr*>(&m_addr);
}

socklen_t IPv4NetAddr::get_sock_len() const noexcept { return sizeof(m_addr); }

int IPv4NetAddr::get_family() const noexcept { return AF_INET; }

std::string IPv4NetAddr::to_string() const noexcept {
    return std::string { fmt::format("{}:{}", m_ip, m_port) };
}
} // namespace rpc