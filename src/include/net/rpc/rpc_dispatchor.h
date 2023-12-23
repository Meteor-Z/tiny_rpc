/*
rpc分发器。将接收到的包分开出去

service OrderService {
    rpc make_order(makeOrderRequest) returns (OrderResponse);
    rpc query_order(queryOrderRequest) returns (queryOrderResponse);
}

1. 注册一个 OrderService 对象
2. 从OrderService 对象中得到 service.method_name， 根据这个name，从中 找到方法 func
3. 找到对应的 request type 以及 response type
4. 将请求体里面的 pb_data 反序列化为 request type 对象，声明一个空的 response type 的对象
5. func(request, response)
6. 将 response 对象序列化成 pb_data, 做 encode 然后塞入 buffer 里面，就会发送回包了。
*/
#ifndef RPC_NET_RPC_RPC_DISPATCHOR_H
#define RPC_NET_RPC_RPC_DISPATCHOR_H

#include <memory>
#include <string>
#include <map>
#include "net/coder/abstract_protocol.h"
#include "google/protobuf/service.h"
#include "net/coder/protobuf_protocol.h"
#include "common/error_code.h"

namespace rpc {
class RpcDispatcher {
public:
    // 进行分发
    void dispatcher(std::shared_ptr<AbstractProtocol> request,
                    std::shared_ptr<AbstractProtocol> response);

    // 将服务进行注册
    void register_service(std::shared_ptr<google::protobuf::Service> service);

    // 设置 错误信息
    void set_protocol_error(std::shared_ptr<ProtobufProtocol> message, int32_t error_code, const std::string& error_info);
private:
    // 解析方法名称
    // 将 full_name 的名称解析到 service_name 和 method_name
    // 如果不能转换，就返回 false, 反之， 就是 true
    bool parse_service_full_name(const std::string& full_name, std::string& service_name, std::string& method_name);
private:
    // 这里的 Service 就是 上面的 OrderService,
    std::map<std::string, std::shared_ptr<google::protobuf::Service>>
        m_service_map; // 存储的是 Service 对象
};
} // namespace rpc

#endif