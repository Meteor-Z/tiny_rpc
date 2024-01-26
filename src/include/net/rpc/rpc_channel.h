/**
 * @file rpc_channel.h
 * @author lzc (liuzechen.coder@qq.com)
 * @brief 客户端进行通讯用的
 * @version 0.1
 * @date 2024-01-26
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef RPC_NET_RPC_RPC_CHANNEL_H
#define RPC_NET_RPC_RPC_CHANNEL_H

#include "google/protobuf/service.h"
#include "net/tcp/ipv4_net_addr.h"

namespace rpc {
class RpcChannel : public google::protobuf::RpcChannel {
public:
    /**
     * @brief Construct a new Rpc Channel object
     * 
     * @param peer_addr 对端地址
     */
    RpcChannel(std::shared_ptr<IPv4NetAddr> peer_addr);
    
    /**
     * @brief Destroy the Rpc Channel object
     * 
     */
    ~RpcChannel() = default;

    /**
     * @brief
     *
     * @param method
     * @param controller
     * @param request
     * @param response
     * @param done
     */
    void CallMethod(const google::protobuf::MethodDescriptor* method,
                    google::protobuf::RpcController* controller,
                    const google::protobuf::Message* request,
                    google::protobuf::Message* response,
                    google::protobuf::Closure* done) override;

private:
    std::shared_ptr<IPv4NetAddr> m_local_addr;
    std::shared_ptr<IPv4NetAddr> m_peer_addr;
};
} // namespace rpc
#endif