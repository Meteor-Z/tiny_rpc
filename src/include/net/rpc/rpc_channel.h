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
#include "net/tcp/tcp_client.h"
#include "net/time/time_event.h"
#include <google/protobuf/message.h>
#include <google/protobuf/stubs/callback.h>
#include <memory>

namespace rpc {
class RpcChannel : public google::protobuf::RpcChannel, public std::enable_shared_from_this<RpcChannel> {
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
    ~RpcChannel();

    /**
     * @brief 初始化方法，保存各个对象的智能指针
     *
     * @param controller
     * @param req
     * @param rsp
     * @param done
     */
    void init(std::shared_ptr<google::protobuf::RpcController> controller,
              std::shared_ptr<google::protobuf::Message> req, std::shared_ptr<google::protobuf::Message> rsp,
              std::shared_ptr<google::protobuf::Closure> done);

    /**
     * @brief
     *
     * @param method
     * @param controller
     * @param request
     * @param response
     * @param done
     */
    void CallMethod(const google::protobuf::MethodDescriptor* method, google::protobuf::RpcController* controller,
                    const google::protobuf::Message* request, google::protobuf::Message* response,
                    google::protobuf::Closure* done) override;

    std::shared_ptr<google::protobuf::RpcController> get_controller();
    std::shared_ptr<google::protobuf::Message> get_request();
    std::shared_ptr<google::protobuf::Message> get_response();
    std::shared_ptr<google::protobuf::Closure> get_closure();
    std::shared_ptr<TcpClient> get_client();

    
    /**
     * @brief Get the timer event object
     * 
     * @return std::shared_ptr<TimerEvent> 
     */
    std::shared_ptr<TimerEvent> get_timer_event();

private:
    std::shared_ptr<IPv4NetAddr> m_local_addr; ///< 本地地址
    std::shared_ptr<IPv4NetAddr> m_peer_addr;  ///< 对方地址
    std::shared_ptr<google::protobuf::RpcController> m_controller { nullptr };
    std::shared_ptr<google::protobuf::Message> m_request { nullptr };
    std::shared_ptr<google::protobuf::Message> m_response { nullptr };
    std::shared_ptr<google::protobuf::Closure> m_closure { nullptr }; ///< 回调函数

    std::shared_ptr<TcpClient> m_client { nullptr };
    std::shared_ptr<TimerEvent> m_timer_evnet { nullptr };
    bool m_is_init { false }; ///< 是否初始化成功
};
} // namespace rpc
#endif