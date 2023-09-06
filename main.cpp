#include "src/common/log.hpp"
#include <exception>
#include <pthread.h>
#include "src/common/config.hpp"

#include <thread>

void test()
{
    for (int i = 0; i < 100; i++)
    {
        rpc::DEBUG_LOG("debug_log_1");
        rpc::ERROR_LOG("debug_log_2");
        rpc::INFO_LOG("DEBUG_LOG_3");
    }
   
}
int main()
{
    rpc::Config::set_global_config("/home/lzc/tiny_rpc/conf/rpc.xml");
    rpc::Logger::init_global_logger();

    rpc::DEBUG_LOG("debug_lone_主");
    rpc::DEBUG_LOG("主线程");

    std::thread t1(test);
    std::thread t2(test);
    std::thread t3(test);
    t1.join();
    t2.join();
    t3.join();
    return 0;
    
}