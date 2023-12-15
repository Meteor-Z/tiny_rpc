#include <cstddef>
#include <google/protobuf/arena.h>
#include <memory>
#include "fmt/core.h"
#include "google/protobuf/service.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "common/log.h"
#include "net/rpc/rpc_dispatchor.h"
#include "net/coder/protobuf_protocol.h"

namespace rpc {
void RpcDispatcher::dispatcher(std::shared_ptr<AbstractProtocol> request,
                               std::shared_ptr<AbstractProtocol> response) {
    // 智能指针转换
    std::shared_ptr<ProtobufProtocol> req_protobuf_protocol =
        std::dynamic_pointer_cast<ProtobufProtocol>(request);
    std::shared_ptr<ProtobufProtocol> rsp_protobuf_protocol =
        std::dynamic_pointer_cast<ProtobufProtocol>(response);

    // 得到全部的方法名称
    std::string method_full_name = req_protobuf_protocol->m_method_name;

    std::string service_name {};
    std::string method_name {};

    // 进行转换
    if (parse_service_full_name(method_full_name, service_name, method_name)) {
        /// TODO: 这里是转发错误的信息
        ERROR_LOG("parse error");
        return;
    }

    auto iter = m_service_map.find(service_name);

    // 找不到
    if (iter == m_service_map.end()) {
        /// TODO: 找不到相关信息
    }

    std::shared_ptr<google::protobuf::Service> service { (*iter).second };
    const google::protobuf::MethodDescriptor* method =
        service->GetDescriptor()->FindMethodByName(method_name);

    if (!method) {
        /// TODO: 之后处理
    }

    // 方法名
    google::protobuf::Message* req_message = service->GetRequestPrototype(method).New();

    // 反序列化， pb_data 反序列化成 req_message;
    if (req_message->ParseFromString(req_protobuf_protocol->m_pb_data)) {
        /// TODO: 失败处理
    }

    INFO_LOG(fmt::format("request id [{}], get rpc request [{}]",
                         req_protobuf_protocol->m_msg_id,
                         req_message->ShortDebugString()));

    google::protobuf::Message* rsp_message = service->GetResponsePrototype(method).New();

    //  virtual void CallMethod(const MethodDescriptor* method,
    //   RpcController* controller, const Message* request,
    //   Message* response, Closure* done)
    service->CallMethod(method, nullptr, req_message, rsp_message, nullptr);

    rsp_protobuf_protocol->m_msg_id = req_protobuf_protocol->m_msg_id;
    rsp_protobuf_protocol->m_method_name = req_protobuf_protocol->m_method_name;
    rsp_protobuf_protocol->m_err_code = 0;
    // 使用序列化
    rsp_protobuf_protocol->m_pb_data =
        rsp_message->SerializeToString(&(rsp_protobuf_protocol->m_pb_data));
    
}

void RpcDispatcher::register_service(std::shared_ptr<google::protobuf::Service> service) {

    std::string service_name = service->GetDescriptor()->full_name();
    m_service_map[std::move(service_name)] = service;
}
bool RpcDispatcher::parse_service_full_name(const std::string& full_name,
                                            std::string& service_name,
                                            std::string& method_name) {
    if (full_name.empty()) {
        return false;
    }

    size_t pos = full_name.find_first_of(".");
    if (pos == full_name.npos) {
        ERROR_LOG(
            fmt::format("not found [.] in fulle name[{}], wrong format", full_name));
        return false;
    }

    // 前面是service_name, 后面的都是 method_name
    service_name = full_name.substr(0, pos);
    method_name = full_name.substr(pos + 1);

    INFO_LOG(
        fmt::format("success parse, full_name = {}, service_naem = {}, method_name = {}",
                    full_name, service_name, method_name));

    return true;
}
} // namespace rpc