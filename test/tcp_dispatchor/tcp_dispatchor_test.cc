#include "net/tcp/tcp_connection.h"
#include "common/log.h"

int main() {
    rpc::LogConfig::SET_GLOBAL_CONFIG("/home/lzc/tiny_rpc/conf/rpc.xml");

    rpc::Logger::INIT_GLOBAL_LOGGER();
    
}