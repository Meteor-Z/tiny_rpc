#include <iostream>
#include <memory>
#include "common/log_config.h"
#include "common/log.h"
#include "net/tcp/ipv4_net_addr.h"
#include "net/tcp/tcp_server.h"

void test_tcp_server() {
    std::shared_ptr<rpc::IPv4NetAddr> addr =
        std::make_shared<rpc::IPv4NetAddr>("127.0.0.1", 1245);
    DEBUG_LOG(fmt::format("addr = {}", addr->to_string()));
    rpc::TcpServer tcp_server(addr);
    tcp_server.start();
}

int main() {
    rpc::LogConfig::SET_GLOBAL_CONFIG("/home/lzc/code/tiny_rpc/conf/rpc.xml");
    rpc::Logger::INIT_GLOBAL_LOGGER();
    test_tcp_server();
}