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
#include "common/error_code.h"

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

    rsp_protobuf_protocol->m_msg_id = req_protobuf_protocol->m_msg_id;
    rsp_protobuf_protocol->m_method_name = req_protobuf_protocol->m_method_name;

    // 进行转换
    if (parse_service_full_name(method_full_name, service_name, method_name)) {
        /// TODO: 这里是转发错误的信息
        /// TODO: 这里是否可以进行优化一下？
        set_protocol_error(rsp_protobuf_protocol, ERROR_PARSE_SERVICE_NAME,
                           "parse service name error");
        return;
    }

    auto iter = m_service_map.find(service_name);

    // service 不存在
    if (iter == m_service_map.end()) {
        /// TODO: 找不到相关信息
        ERROR_LOG(fmt::format("req_id = {}, service name {} not found",
                              req_protobuf_protocol->m_msg_id, service_name));
        set_protocol_error(rsp_protobuf_protocol, ERROR_SERVICE_NOT_FOUND,
                           "service not found");
        return;
    }

    std::shared_ptr<google::protobuf::Service> service { (*iter).second };
    const google::protobuf::MethodDescriptor* method =
        service->GetDescriptor()->FindMethodByName(method_name);

    if (!method) {
        ERROR_LOG(fmt::format("req_id = {}, method {} not found",
                              req_protobuf_protocol->m_msg_id, method_name));
        set_protocol_error(rsp_protobuf_protocol, ERROR_METHOD_NOT_FOUND,
                           "ERROR_METHOD_NOT_FOUND");
        return;
    }

    // 方法名
    google::protobuf::Message* req_message = service->GetRequestPrototype(method).New();

    // 反序列化， pb_data 反序列化成 req_message;
    if (req_message->ParseFromString(req_protobuf_protocol->m_pb_data)) {
        ERROR_LOG(fmt::format("req_id = {}, deserilize error",
                              req_protobuf_protocol->m_msg_id));
        set_protocol_error(rsp_protobuf_protocol, ERROR_FAILED_DESERIALIZE,
                           "deserialize error");
    }

    INFO_LOG(fmt::format("request id [{}], get rpc request [{}]",
                         req_protobuf_protocol->m_msg_id,
                         req_message->ShortDebugString()));

    google::protobuf::Message* rsp_message = service->GetResponsePrototype(method).New();

    //  virtual void CallMethod(const MethodDescriptor* method,
    //   RpcController* controller, const Message* request,
    //   Message* response, Closure* done)
    service->CallMethod(method, nullptr, req_message, rsp_message, nullptr);

    // rsp_protobuf_protocol->m_err_code = 0;
    // 如果序列化成功 ????
    if (rsp_message->SerializeToString(&(rsp_protobuf_protocol->m_pb_data))) {
        ERROR_LOG(
            fmt::format("req_id = {}, serilize error", req_protobuf_protocol->m_msg_id));
        set_protocol_error(rsp_protobuf_protocol, ERROR_FAILED_SERIALIZE,
                           "serialize error");
    }


    rsp_protobuf_protocol->m_err_code = 0;
    // ok 了
    INFO_LOG(fmt::format("req_id = {}, dispatch success, request = {}, response = {}",
                         req_protobuf_protocol->m_msg_id, req_message->ShortDebugString(),
                         rsp_message->ShortDebugString()));
}

void RpcDispatcher::register_service(std::shared_ptr<google::protobuf::Service> service) {

    std::string service_name = service->GetDescriptor()->full_name();
    m_service_map[std::move(service_name)] = service;
}

void RpcDispatcher::set_protocol_error(std::shared_ptr<ProtobufProtocol> message,
                                       int32_t error_code,
                                       const std::string& error_info) {
    message->m_err_code = error_code;
    message->m_err_info = error_info;
    message->m_err_info_len = error_info.size();
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