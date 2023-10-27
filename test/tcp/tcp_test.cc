#include "common/log_config.h"
#include "common/log.h"
#include "net/tcp/net_addr.h"

int main() {
    rpc::LogConfig::SET_GLOBAL_CONFIG("/home/lzc/tiny_rpc/conf/rpc.xml");
    rpc::Logger::INIT_GLOBAL_LOGGER();
    
    rpc::IPNetAddr addr("127.0.0.1", 12345);
    rpc::utils::DEBUG_LOG(addr.to_string());
}