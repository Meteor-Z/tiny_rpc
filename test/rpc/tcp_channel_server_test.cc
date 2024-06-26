#include <memory>
#include <unistd.h>
#include "fmt/core.h"
#include "net/rpc/rpc_dispatchor.h"
#include "net/tcp/tcp_server.h"
#include "order.pb.h" // 导入的proto生成的文件
#include "google/protobuf/service.h"
#include "common/log.h"
#include "net/tcp/ipv4_net_addr.h"
#include "common/log_config.h"

// 继承于 Service
class OrderImpl : public Order {
public:
    void makeOrder(google::protobuf::RpcController* controller,
                   const ::makeOrderRequest* request, ::makeOrderResponse* response,
                   ::google::protobuf::Closure* done) {
        // 测试超时的时间的限制
        // DEBUG_LOG("start sleep 5s");
        // sleep(5);
        // DEBUG_LOG("end sleep 5s");
        if (request->price() > 10) {
            response->set_ret_code(-1);
            response->set_res_info("shahahhaha");
            return;
        }
        response->set_order_id("20230514");
    }
};

void test_tcp_server() {
    std::unique_ptr<rpc::IPv4NetAddr> addr =
        std::make_unique<rpc::IPv4NetAddr>(rpc::LogConfig::GET_GLOBAL_CONFIG()->m_ip,
                                           rpc::LogConfig::GET_GLOBAL_CONFIG()->m_port);

    std::cout << rpc::LogConfig::GET_GLOBAL_CONFIG()->m_ip << std::endl;
    std::cout << rpc::LogConfig::GET_GLOBAL_CONFIG()->m_port << std::endl;

    DEBUG_LOG(fmt::format("create addr = {}", addr->to_string()));

    std::unique_ptr<rpc::TcpServer> tcp_server =
        std::make_unique<rpc::TcpServer>(std::move(addr));

    tcp_server->start();
}
int main() {
    rpc::LogConfig::SET_GLOBAL_CONFIG("/home/lzc/code/tiny_rpc/conf/rpc.xml");

    rpc::Logger::INIT_GLOBAL_LOGGER();

    // 注册方法
    std::shared_ptr<OrderImpl> order_ptr = std::make_shared<OrderImpl>();

    rpc::RpcDispatcher::GET_RPC_DISPATCHER()->register_service(order_ptr);

    // test_tcp_dispatchor();

    test_tcp_server();
}