#include "src/common/log.hpp"
#include <pthread.h>
#include "src/common/config.hpp"

void* fun(void*) 
{
    rpc::DEBUG_BLOG("多线程");
    return NULL;
}
int main()
{
    rpc::Config confg("/home/lzc/tiny_rpc/conf/rpc.xml"); // 配置文件
    // pthread_t thread;
    // pthread_create(&thread, NULL, &fun, NULL); 
    // rpc::DEBUG_BLOG("Args args...");
}