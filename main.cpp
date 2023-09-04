#include "src/common/log.hpp"
#include <pthread.h>
#include "src/common/config.hpp"


int main()
{
    // rpc::Config confg("/home/lzc/tiny_rpc/conf/rpc.xml"); // 配置文件
    rpc::Config::set_global_config("/home/lzc/tiny_rpc/conf/rpc.xml");
    rpc::DEBUG_LOG("hello");
}