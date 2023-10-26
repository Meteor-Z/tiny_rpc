#include <iostream>
#include <memory>
#include "common/config.h"
#include "common/log.h"
#include "net/tcp/net_addr.h"
#include "net/tcp/tcp_server.h"


void test_tcp_server() {
    std::shared_ptr<rpc::IPNetAddr> addr = std::make_shared<rpc::IPNetAddr>("127.0.0.1", 1245);
    rpc::utils::DEBUG_LOG(fmt::format("addr = {}", addr->to_string()));
    std::cout << "1 -----" << std::endl;
    rpc::TcpServer tcp_server(addr);
    tcp_server.start();
}

int main() {
    rpc::Config::set_global_config("/home/lzc/tiny_rpc/conf/rpc.xml");
    rpc::Logger::init_global_logger();
    test_tcp_server(); 
}