#include "common/config.hpp"
#include "common/log.hpp"
#include "net/tcp/net_addr.hpp"

int main() {
    rpc::Config::set_global_config("/home/lzc/tiny_rpc/conf/rpc.xml");
    rpc::Logger::init_global_logger();
    
    rpc::IPNetAddr addr("127.0.0.1", 12345);
    rpc::utils::DEBUG_LOG(addr.to_string());
}