#include <iostream>
#include <memory>
#include "common/log_config.h"
#include "common/log.h"
#include "net/tcp/net_addr.h"
#include "net/tcp/tcp_server.h"


void test_tcp_server() {
    std::shared_ptr<rpc::IPv4NetAddr> addr = std::make_shared<rpc::IPv4NetAddr>("127.0.0.1", 1245);
    rpc::utils::DEBUG_LOG(fmt::format("addr = {}", addr->to_string()));
    std::cout << "1 -----" << std::endl;
    rpc::TcpServer tcp_server(addr);
    tcp_server.start();
}

int main() {
    rpc::LogConfig::SET_GLOBAL_CONFIG("/home/lzc/tiny_rpc/conf/rpc.xml");
    rpc::Logger::INIT_GLOBAL_LOGGER();
    test_tcp_server(); 
}