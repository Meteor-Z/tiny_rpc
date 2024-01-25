#include <cstddef>
#include <memory>
#include "common/log.h"
#include "fmt/core.h"
#include "google/protobuf/service.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "net/rpc/rpc_dispatchor.h"
#include "net/rpc/rpc_controller.h"
#include "net/coder/protobuf_protocol.h"
#include "common/error_code.h"
#include "net/tcp/tcp_connection.h"

namespace rpc {
static std::shared_ptr<RpcDispatcher> g_rpc_dispatchor { nullptr };
std::shared_ptr<RpcDispatcher> RpcDispatcher::GET_RPC_DISPATCHER() {
    if (!g_rpc_dispatchor) {
        g_rpc_dispatchor = std::make_shared<RpcDispatcher>();
        DEBUG_LOG("first make_shared<RpcDispatcher>()");
    }
    return g_rpc_dispatchor;
}
void RpcDispatcher::dispatcher(std::shared_ptr<AbstractProtocol> request,
                               std::shared_ptr<AbstractProtocol> response,
                               TcpConnection* conection) {

    // 智能指针转换
    std::shared_ptr<ProtobufProtocol> req_protobuf_protocol =
        std::dynamic_pointer_cast<ProtobufProtocol>(request);

    std::shared_ptr<ProtobufProtocol> rsp_protobuf_protocol =
        std::dynamic_pointer_cast<ProtobufProtocol>(response);

    // 得到全部的方法名称
    std::string method_full_name = req_protobuf_protocol->m_method_name;

    std::string service_name {};
    std::string method_name {};

    // 先进行赋值
    rsp_protobuf_protocol->m_msg_id = req_protobuf_protocol->m_msg_id;
    rsp_protobuf_protocol->m_method_name = req_protobuf_protocol->m_method_name;

    // 进行转换, 如果转换失败
    if (!parse_service_full_name(method_full_name, service_name, method_name)) {
        // 设置
        ERROR_LOG("parse service name error");
        set_protubuf_error(rsp_protobuf_protocol, ERROR_PARSE_SERVICE_NAME,
                           "parse service name error");
        return;
    }

    DEBUG_LOG(fmt::format("method_name = {}", method_name));
    auto iter = m_service_map.find(service_name);

    // service不存在
    if (iter == m_service_map.end()) {
        ERROR_LOG("service not found");
        set_protubuf_error(rsp_protobuf_protocol, ERROR_SERVICE_NOT_FOUND,
                           "service not found");
        return;
    }
    DEBUG_LOG("1 到这里是对的");
    std::shared_ptr<google::protobuf::Service> service { (*iter).second };

    // 这里有可能发生内存泄漏
    const google::protobuf::MethodDescriptor* method =
        service->GetDescriptor()->FindMethodByName(method_name);

    // may be is error
    if (method == nullptr) {
        ERROR_LOG("error_method not found");
        set_protubuf_error(rsp_protobuf_protocol, ERROR_METHOD_NOT_FOUND,
                           "method not found");
        return;
    }

    // 方法名
    google::protobuf::Message* req_message = service->GetRequestPrototype(method).New();

    DEBUG_LOG("2 到这里是对的");

    // 反序列化， pb_data 反序列化成 req_message;
    // 反序列化错误
    if (!req_message->ParseFromString(req_protobuf_protocol->m_pb_data)) {
        ERROR_LOG(fmt::format("{} {} {} | deserilize error",
                              req_protobuf_protocol->m_msg_id,
                              req_protobuf_protocol->m_method_name, service_name));
        set_protubuf_error(rsp_protobuf_protocol, ERROR_FAILED_DESERIALIZE,
                           "serialize error");
        if (!req_message) {
            delete req_message;
            req_message = nullptr;
        }
        return;
    }
    DEBUG_LOG("3 到这里是对的");
    INFO_LOG(fmt::format("request id [{}], get rpc request, info =[{}]",
                         req_protobuf_protocol->m_msg_id,
                         req_message->ShortDebugString()));

    google::protobuf::Message* rsp_message = service->GetResponsePrototype(method).New();
    DEBUG_LOG("4 这里是对的");

    //  virtual void CallMethod(const MethodDescriptor* method,
    //   RpcController* controller, const Message* request,
    //   Message* response, Closure* done)
    // 通过这个来调用远程方法的
    /// TODO: 进行补充

    RpcController rpc_controller;
    rpc_controller.set_local_addr(conection->get_local_addr());
    rpc_controller.set_peer_addr(conection->get_peer_addr());
    rpc_controller.set_req_id(req_protobuf_protocol->m_msg_id);

    DEBUG_LOG(fmt::format("local_addr = {}, peer_addr = {}, req_id = {}",
                          rpc_controller.get_local_addr()->to_string(),
                          rpc_controller.get_peer_addr()->to_string(),
                          rpc_controller.get_req_id()));

    DEBUG_LOG("5 这里是对的");
    /// TODO: 这里是错误的
    service->CallMethod(method, &rpc_controller, req_message, rsp_message, nullptr);

    // 使用序列化
    if (!rsp_message->SerializeToString(&(rsp_protobuf_protocol->m_pb_data))) {
        ERROR_LOG(fmt::format("{} serialize error", req_protobuf_protocol->m_msg_id,
                              rsp_message->ShortDebugString()));
        set_protubuf_error(rsp_protobuf_protocol, ERROR_SERVICE_NOT_FOUND,
                           "serilize error");

        if (!req_message) {
            delete req_message;
            req_message = nullptr;
        }
        if (!rsp_message) {
            delete rsp_message;
            rsp_message = nullptr;
        }
        return;
    }

    // 最后的错误代码，表示正确
    rsp_protobuf_protocol->m_err_code = 0;
    // dispatch success
    INFO_LOG(fmt::format("{} | dispatchor success!!!, request = {}, response = {}",
                         request->m_msg_id, req_message->ShortDebugString(),
                         req_message->ShortDebugString()));
    delete req_message;
    req_message = nullptr;

    delete rsp_message;
    rsp_message = nullptr;
}

void RpcDispatcher::register_service(std::shared_ptr<google::protobuf::Service> service) {

    std::string service_name = service->GetDescriptor()->full_name();
    m_service_map[service_name] = service;
}

void RpcDispatcher::set_protubuf_error(std::shared_ptr<ProtobufProtocol> msg,
                                       int32_t err_code, const std::string err_info) {
    msg->m_err_code = err_code;
    msg->m_err_info_len = err_info.length();
    msg->m_err_info = std::move(err_info);
}
bool RpcDispatcher::parse_service_full_name(const std::string& full_name,
                                            std::string& service_name,
                                            std::string& method_name) {
    if (full_name.empty()) {
        return false;
    }

    size_t pos = full_name.find_first_of(".");
    // 没有找到
    if (pos == full_name.npos) {
        ERROR_LOG(
            fmt::format("not found [.] in fulle name[{}], wrong format", full_name));
        return false;
    }

    // 解析成功
    // 前面是service_name, 后面的都是 method_name
    service_name = full_name.substr(0, pos);
    method_name = full_name.substr(pos + 1);

    INFO_LOG(
        fmt::format("success parse, full_name = {}, service_naem = {}, method_name = {}",
                    full_name, service_name, method_name));

    return true;
}
} // namespace rpc