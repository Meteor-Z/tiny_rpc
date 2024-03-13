#include <memory>
#include "fmt/core.h"
#include "net/rpc/rpc_channel.h"
#include "net/rpc/rpc_closure.h"
#include "net/rpc/rpc_controller.h"
#include "order.pb.h" // 导入的proto生成的文件
#include "common/log.h"
#include "net/tcp/ipv4_net_addr.h"
#include "common/log_config.h"

// void test_tcp_dispatchor_client() {
//     std::shared_ptr<rpc::IPv4NetAddr> addr =
//         std::make_shared<rpc::IPv4NetAddr>("127.0.0.1", 12347);
//     std::shared_ptr<rpc::TcpClient> client = std::make_shared<rpc::TcpClient>(addr);

//     client->connect([addr, &client]() {
//         DEBUG_LOG(fmt::format("connect to {} success", addr->to_string()));
//         std::shared_ptr<rpc::ProtobufProtocol> message =
//             std::make_shared<rpc::ProtobufProtocol>();
//         message->m_msg_id = "114514";
//         message->m_pb_data = "test pb data";

//         makeOrderRequest request;
//         request.set_price(100);
//         request.set_goods("apple");

//         // 序列化
//         if (!request.SerializeToString(&(message->m_pb_data))) {
//             ERROR_LOG("serialize error");
//             return;
//         }

//         message->m_method_name = "Order.makeOrder";

//         client->write_message(message,
//                               [](std::shared_ptr<rpc::AbstractProtocol> msg_ptr) {
//                                   DEBUG_LOG("send mssage success");
//                               });
//         DEBUG_LOG("YES,下面是收到的信息");
//         client->read_message(
//             "114514", [](std::shared_ptr<rpc::AbstractProtocol> msg_ptr) {
//                 std::shared_ptr<rpc::ProtobufProtocol> message =
//                     std::dynamic_pointer_cast<rpc::ProtobufProtocol>(msg_ptr);
//                 DEBUG_LOG(fmt::format("req_id {}, get response {}", message->m_msg_id,
//                                       message->m_pb_data));

//                 makeOrderResponse response;
//                 if (!response.ParseFromString(message->m_pb_data)) {
//                     ERROR_LOG("寄了");
//                     return;
//                 }
//                 INFO_LOG(fmt::format("message success, response = {}",
//                                      response.ShortDebugString()));
//             });
//     });
// }

void test_rpc_channel_client() {
    std::shared_ptr<rpc::IPv4NetAddr> addr = std::make_shared<rpc::IPv4NetAddr>("127.0.0.1", 12347);
    std::shared_ptr<rpc::RpcChannel> channel = std::make_shared<rpc::RpcChannel>(addr);

    std::shared_ptr<makeOrderRequest> request = std::make_shared<makeOrderRequest>();
    request->set_price(100);
    request->set_goods("apple");

    std::shared_ptr<makeOrderResponse> response = std::make_shared<makeOrderResponse>();

    std::shared_ptr<rpc::RpcController> controller = std::make_shared<rpc::RpcController>();
    controller->set_msg_id("114514");

    std::shared_ptr<rpc::RpcClosure> closure =
        std::make_shared<rpc::RpcClosure>([request, response, channel, controller]() mutable {
            // 只有表示不等于0， 才算是调用失败
            if (controller->get_error_code() == 0) {
                INFO_LOG(fmt::format("call rpc success, request = {}, response = {}", request->ShortDebugString(),
                                     response->ShortDebugString()));

                // 业务逻辑
            } else {
                ERROR_LOG(fmt::format("call rpc failed, request = {} ,error code = {}, error_info = {}",
                                      request->ShortDebugString(), controller->get_error_code(),
                                      controller->get_error_info()));
                // 退出 loop循环
                INFO_LOG("exit eventloop")
                channel->get_client()->stop();
                channel.reset();
            }
        });

    controller->set_timeout(10000);
    channel->init(controller, request, response, closure);
    Order_Stub stub(channel.get());

    // controller->set_timeout(5000);
    stub.makeOrder(controller.get(), request.get(), response.get(), closure.get());
}

int main() {
    rpc::LogConfig::SET_GLOBAL_CONFIG("/home/lzc/code/tiny_rpc/conf/rpc.xml");

    rpc::Logger::INIT_GLOBAL_LOGGER();

    // test_tcp_dispatchor_client();
    test_rpc_channel_client();
}