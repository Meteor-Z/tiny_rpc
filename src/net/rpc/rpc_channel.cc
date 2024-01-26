#include "net/rpc/rpc_channel.h"
#include "common/error_code.h"
#include "common/msg_id_utils.h"
#include "net/coder/abstract_protocol.h"
#include "net/coder/protobuf_protocol.h"
#include "net/rpc/rpc_controller.h"
#include "common/log.h"
#include "net/tcp/tcp_client.h"
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

namespace rpc {
RpcChannel::RpcChannel(std::shared_ptr<IPv4NetAddr> peer_addr) : m_peer_addr(peer_addr) {}

void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                            google::protobuf::RpcController* controller,
                            const google::protobuf::Message* request,
                            google::protobuf::Message* response,
                            google::protobuf::Closure* done) {
    std::shared_ptr<ProtobufProtocol> req_protocol = std::make_shared<ProtobufProtocol>();
    RpcController* my_controller = dynamic_cast<RpcController*>(controller);
    if (!my_controller) {
        ERROR_LOG("RpcController cconver error");
        return;
    }
    // 如果没有，就自己生成一个
    /// TODO: 这里需要改一下
    if (my_controller->get_msg_id().empty()) {
        req_protocol->m_msg_id = rpc::utils::MsgIdUtils::gen_msg_id();
        my_controller->set_msg_id(req_protocol->m_msg_id);
    } else {
        req_protocol->m_msg_id = my_controller->get_msg_id();
    }

    req_protocol->m_method_name = method->full_name();

    INFO_LOG(fmt::format("msg_id = {} | method name = {}", req_protocol->m_msg_id,
                         req_protocol->m_method_name));

    if (!request->SerializeToString(&(req_protocol->m_pb_data))) {
        std::string error_info = "failed to serizlize";
        my_controller->set_error(ERROR_FAILED_SERIALIZE, error_info);

        ERROR_LOG(fmt::format("{} | request = {}, info = {}", req_protocol->m_msg_id,
                              error_info, request->ShortDebugString()));
    }

    std::shared_ptr<TcpClient> client = std::make_shared<TcpClient>(m_peer_addr);
    client->connect([client, req_protocol, done]() {
        client->write_message(req_protocol, [&client, req_protocol,
                                             done](std::shared_ptr<AbstractProtocol>) {
            INFO_LOG(fmt::format(
                "{} | send request success, call method name {}, origin request {}",
                req_protocol->m_msg_id, req_protocol->m_method_name));

            // 回包
            client->read_message(
                req_protocol->m_msg_id, [done](std::shared_ptr<AbstractProtocol> msg) {
                    std::shared_ptr<ProtobufProtocol> rsp_protocol =
                        std::make_shared<ProtobufProtocol>(msg);
                    DEBUG_LOG(fmt::format("msg_id {}, get response, method name = {}",
                                          rsp_protocol->m_msg_id,
                                          rsp_protocol->m_method_name));

                    if (done) {
                        done->Run();
                    }
                });
        });
    });
}
} // namespace rpc