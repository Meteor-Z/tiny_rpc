#include "common/config.h"
#include "common/log.h"
#include "net/tcp/net_addr.h"

int main() {
    rpc::Config::set_global_config("/home/lzc/tiny_rpc/conf/rpc.xml");
    rpc::Logger::init_global_logger();
    
    rpc::IPNetAddr addr("127.0.0.1", 12345);
    rpc::utils::DEBUG_LOG(addr.to_string());
}