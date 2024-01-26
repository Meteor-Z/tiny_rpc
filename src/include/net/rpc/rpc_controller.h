/**
 * @file rpc_controller.h
 * @author lzc (liuzechen.coder@qq.com)
 * @brief 继承google::protobuf::RpcController的controller配置信息
 * @version 0.1
 * @date 2024-01-21
 *
 * @copyright Copyright (c) 2024
 *
 */
 
#ifndef RPC_NET_RPC_RPC_CONTROLLER_H
#define RPC_NET_RPC_RPC_CONTROLLER_H

#include <cstdint>
#include <string>
#include "google/protobuf/service.h"
#include "google/protobuf/stubs/callback.h"
#include "net/tcp/ipv4_net_addr.h"

namespace rpc {

/**
 * @brief 继承protobuf::RpcController的
 *
 */
class RpcController : public google::protobuf::RpcController {
public:
    RpcController() = default;
    ~RpcController() = default;

    /**
     * @brief 将其成员变量全部变成初始值
     *
     */
    void Reset() override;

    /**
     * @brief 查看rpc调用是否成功
     *
     * @return true 失败
     * @return false 成功
     */
    bool Failed() const override;

    /**
     * @brief 如果 Failed() 返回true, 即失败了，那么就返回错误信息
     *
     * @return std::string 错误信息
     */
    std::string ErrorText() const override;

    /**
     * @brief 设置取消
     *
     */
    void StartCancel() override;

    /**
     * @brief 设置错误信息
     *
     * @param reason 错误信息
     */
    void SetFailed(const std::string& reason) override;

    /**
     * @brief 是否取消
     *
     * @return true 取消掉
     * @return false 不取消
     */
    bool IsCanceled() const override;

    /**
     * @brief
     *
     * @param callback
     */
    void NotifyOnCancel(google::protobuf::Closure* callback) override;

    /**
     * @brief 设置错误码和错误信息
     *
     * @param error_code 错误码
     * @param error_info 错误信息
     */
    void set_error(int32_t error_code, const std::string& error_info);

    /**
     * @brief 得到错误码
     *
     * @return int32_t 错误码
     */
    int32_t get_error_code();

    /**
     * @brief 获取 error_info
     *
     * @return std::string error_info
     */
    std::string get_error_info();

    /**
     * @brief 设置req_id(请求id)
     *
     * @param req_id 请求id
     */
    void set_msg_id(const std::string& msg_id);

    /**
     * @brief 获取req_id(请求id)
     *
     * @return std::string
     */
    std::string get_msg_id();

    /**
     * @brief Set the local addr object
     * 
     * @param local_addr 
     */
    void set_local_addr(std::shared_ptr<IPv4NetAddr> local_addr);
    
    /**
     * @brief Get the local addr object
     * 
     * @return std::shared_ptr<IPv4NetAddr> 
     */
    std::shared_ptr<IPv4NetAddr> get_local_addr();

    /**
     * @brief Set the peer addr object
     * 
     * @param peer_addr 
     */
    void set_peer_addr(std::shared_ptr<IPv4NetAddr> peer_addr);

    /**
     * @brief Get the peer addr object
     * 
     * @return std::shared_ptr<IPv4NetAddr> 
     */
    std::shared_ptr<IPv4NetAddr> get_peer_addr();

    /**
     * @brief Set the timeout object
     * 
     * @param timeout 
     */
    void set_timeout(int32_t timeout);

    /**
     * @brief Get the timeout object
     * 
     * @return int32_t 
     */
    int32_t get_timeout();

private:
    int m_error_code { 0 };      ///< 错误信息码
    std::string m_error_info {}; ///<  错误信息
    std::string m_msg_id {};     ///< 请求id
    bool m_is_failed { false };  ///< 是否是失败的
    bool m_is_cancled { false }; ///< 是否取消的

    std::shared_ptr<IPv4NetAddr> m_local_addr; ///< 本地地址
    std::shared_ptr<IPv4NetAddr> m_peer_addr;  ///< 对端地址

    int m_timeout_ms { 1000 }; ///< 超时时间，单位是毫秒
};
} // namespace rpc
#endif