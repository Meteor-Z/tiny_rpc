#include <memory>
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "common/error_code.h"
#include "common/msg_id_utils.h"
#include "common/log.h"
#include "net/coder/abstract_protocol.h"
#include "net/coder/protobuf_protocol.h"
#include "net/rpc/rpc_channel.h"
#include "net/rpc/rpc_controller.h"
#include "net/tcp/tcp_client.h"
#include "net/time/time_event.h"

namespace rpc {
RpcChannel::RpcChannel(std::shared_ptr<IPv4NetAddr> peer_addr) : m_peer_addr(peer_addr) {
    m_client = std::make_shared<TcpClient>(m_peer_addr);
}
RpcChannel::~RpcChannel() { INFO_LOG("~RpcChannel()"); }

void RpcChannel::init(std::shared_ptr<google::protobuf::RpcController> controller,
                      std::shared_ptr<google::protobuf::Message> req,
                      std::shared_ptr<google::protobuf::Message> rsp,
                      std::shared_ptr<google::protobuf::Closure> done) {
    if (m_is_init) {
        return;
    }

    m_controller = controller;
    m_request = req;
    m_response = rsp;
    m_closure = done;
    m_is_init = true;
}

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

    if (!m_is_init) {
        std::string error_info = "RpcChannel not init";
        ERROR_LOG("RpcChannel not init");
        my_controller->set_error(ERROR_RPC_CHANNEL_NOT_INIT, error_info);
        return;
    }
    // 在发送之前，将其序列化，
    if (!request->SerializeToString(&(req_protocol->m_pb_data))) {
        std::string error_info = "failed to serizlize";
        my_controller->set_error(ERROR_FAILED_SERIALIZE, error_info);

        ERROR_LOG(fmt::format("{} | request = {}, info = {}", req_protocol->m_msg_id,
                              error_info, request->ShortDebugString()));
        return;
    }

    std::shared_ptr<RpcChannel> channel = shared_from_this();

    // 超时时间
    m_timer_evnet = std::make_shared<TimerEvent>(
        my_controller->get_timeout(), false, [my_controller, channel]() mutable {
            // 如果超时的话，要执行什么任务
            // 取消任务，并且设置错误码
            my_controller->StartCancel();
            my_controller->set_error(ERROR_RPC_CALL_TIMEOUT,
                                     "rpc call timeout" +
                                         std::to_string(my_controller->get_timeout()));
            if (channel->get_closure()) {
                channel->get_closure()->Run();
            }
            // channel.reset();
        });

    // 添加定时任务
    m_client->add_timer_event(m_timer_evnet);

    DEBUG_LOG(fmt::format("peer_addr = {}", m_peer_addr->to_string()));

    m_client->connect([req_protocol, channel, my_controller]() mutable {
        // RpcController* my_controller =
        //     dynamic_cast<RpcController*>(channel->get_controller().get());

        // error_code 不等于0，说明连接失败
        if (channel->get_client()->get_connect_error_code() != 0) {
            my_controller->set_error(channel->get_client()->get_connect_error_code(),
                                     channel->get_client()->get_connect_error_info());

            ERROR_LOG(fmt::format("error, error code {}, error info = {}",
                                  my_controller->get_error_code(),
                                  my_controller->get_error_info()));
            return;
        }

        channel->get_client()->write_message(
            req_protocol, [req_protocol, channel,
                           my_controller](std::shared_ptr<AbstractProtocol>) mutable {
                INFO_LOG(fmt::format("{} | send request success, call method name {}",
                                     req_protocol->m_msg_id,
                                     req_protocol->m_method_name));

                // 回包
                channel->get_client()->read_message(
                    req_protocol->m_msg_id,
                    [channel,
                     my_controller](std::shared_ptr<AbstractProtocol> msg) mutable {
                        std::shared_ptr<ProtobufProtocol> rsp_protocol =
                            std::dynamic_pointer_cast<ProtobufProtocol>(msg);

                        DEBUG_LOG(fmt::format("msg_id {}, get response, method name = {}",
                                              rsp_protocol->m_msg_id,
                                              rsp_protocol->m_method_name));

                        // RpcController* my_controller =
                        //     dynamic_cast<RpcController*>(channel->get_controller().get());

                        // 取消这个定时任务，表明定时任务成功
                        channel->get_timer_event()->set_cancel(true);

                        if (!(channel->get_response()->ParseFromString(
                                rsp_protocol->m_pb_data))) {
                            ERROR_LOG("deserizlize error");
                            my_controller->set_error(ERROR_FAILED_SERIALIZE,
                                                     "serizlize error");
                            return;
                        }

                        if (rsp_protocol->m_err_code != 0) {
                            ERROR_LOG(fmt::format("{} | error", rsp_protocol->m_msg_id));
                            my_controller->set_error(rsp_protocol->m_err_code,
                                                     rsp_protocol->m_err_info);
                            return;
                        }

                        // 如果没有被取消掉，并且有的话，才会执行这个任务
                        if ((!my_controller->IsCanceled()) && channel->get_closure()) {
                            channel->get_closure()->Run();
                        }

                        channel.reset();
                    });
            });
    });
}

std::shared_ptr<google::protobuf::RpcController> RpcChannel::get_controller() {
    return m_controller;
}
std::shared_ptr<google::protobuf::Message> RpcChannel::get_request() { return m_request; }
std::shared_ptr<google::protobuf::Message> RpcChannel::get_response() {
    return m_response;
}
std::shared_ptr<google::protobuf::Closure> RpcChannel::get_closure() { return m_closure; }
std::shared_ptr<TcpClient> RpcChannel::get_client() { return m_client; }
std::shared_ptr<TimerEvent> RpcChannel::get_timer_event() { return m_timer_evnet; }
} // namespace rpc