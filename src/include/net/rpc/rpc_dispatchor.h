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
#include "google/protobuf/service.h"
#include "net/coder/abstract_protocol.h"
#include "net/coder/protobuf_protocol.h"

namespace rpc {
// 前置声明
class TcpConnection;
class RpcDispatcher {
public:
    static std::shared_ptr<RpcDispatcher> GET_RPC_DISPATCHER();
public:
    /**
     * @brief 进行分发,在这里就会将数据反序列化
     *
     * @param request
     * @param response
     */
    void dispatcher(std::shared_ptr<AbstractProtocol> request,
                    std::shared_ptr<AbstractProtocol> response, TcpConnection* conection);

    /**
     * @brief 注册 service
     *
     * @param service gopogle:protobuf:Service 的对象
     */
    void register_service(std::shared_ptr<google::protobuf::Service> service);

    /**
     * @brief 设置错误信息
     *
     * @param msg
     * @param err_code 错误信息玛
     * @param err_info 错误消息
     */
    void set_protubuf_error(std::shared_ptr<ProtobufProtocol> msg, int32_t err_code,
                            const std::string err_info);

private:
    /**
     * @brief 解析方法名称，将full_name的名称解析成 service_name 和 method_name
     *
     * @param full_name 全部的名称
     * @param service_name 解析出来的service_name
     * @param method_name 解析出来的full_name
     * @return true 解析出来了，并且给了service_name 和 method_name
     * @return false 没有解析出来，这时候并不应该使用servic_name 和 method_name
     */
    bool parse_service_full_name(const std::string& full_name, std::string& service_name,
                                 std::string& method_name);

private:
    // 这里的 Service 就是 上面的 OrderService,
    std::map<std::string, std::shared_ptr<google::protobuf::Service>>
        m_service_map; ///< 存储的是 Service 对象
};
} // namespace rpc

#endif