#include <memory>
#include "fmt/core.h"
#include "net/rpc/rpc_dispatchor.h"
#include "net/tcp/tcp_server.h"
#include "order.pb.h" // 导入的proto生成的文件
#include "google/protobuf/service.h"
#include "common/log.h"
#include "net/coder/protobuf_protocol.h"
#include "net/coder/abstract_protocol.h"
#include "net/tcp/ipv4_net_addr.h"
#include "net/tcp/tcp_client.h"
#include "net/tcp/tcp_connection.h"
#include "common/log_config.h"
#include "net/coder/protobuf_protocol.h"

void test_tcp_dispatchor_client() {
    std::shared_ptr<rpc::IPv4NetAddr> addr =
        std::make_shared<rpc::IPv4NetAddr>("127.0.0.1", 12347);
    std::shared_ptr<rpc::TcpClient> client = std::make_shared<rpc::TcpClient>(addr);

    client->connect([addr, &client]() {
        DEBUG_LOG(fmt::format("connect to {} success", addr->to_string()));
        std::shared_ptr<rpc::ProtobufProtocol> message =
            std::make_shared<rpc::ProtobufProtocol>();
        message->m_msg_id = "114514";
        message->m_pb_data = "test pb data";

        makeOrderRequest request;
        request.set_price(100);
        request.set_goods("apple");

        // 序列化
        if (!request.SerializeToString(&(message->m_pb_data))) {
            ERROR_LOG("serialize error");
            return;
        }

        message->m_method_name = "Order.makeOrder";

        client->write_message(message,
                              [](std::shared_ptr<rpc::AbstractProtocol> msg_ptr) {
                                  DEBUG_LOG("send mssage success");
                              });
    });

    client->read_message("114514", [](std::shared_ptr<rpc::AbstractProtocol> msg_ptr) {
        std::shared_ptr<rpc::ProtobufProtocol> message =
            std::dynamic_pointer_cast<rpc::ProtobufProtocol>(msg_ptr);
        DEBUG_LOG(fmt::format("req_id {}, get response {}", message->m_msg_id,
                              message->m_pb_data));

        makeOrderResponse response;
        if (!response.ParseFromString(message->m_pb_data)) {
            ERROR_LOG("寄了");
            return;
        }
        INFO_LOG(
            fmt::format("message success, response = {}", response.ShortDebugString()));
    });
}

int main() {
    rpc::LogConfig::SET_GLOBAL_CONFIG("/home/lzc/code/tiny_rpc/conf/rpc.xml");

    rpc::Logger::INIT_GLOBAL_LOGGER();

    test_tcp_dispatchor_client();
}