/**
 * @file rpc_closure.h
 * @author lzc (liuzechen.coder@qq.com)
 * @brief 回调函数
 * @version 0.1
 * @date 2024-03-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
 
#pragma once

#ifndef RPC_NET_RPC_RPC_CLOSURE_H
#define RPC_NET_RPC_RPC_CLOSURE_H

#include <functional>
#include "google/protobuf/stubs/callback.h"

namespace rpc {
class RpcClosure : public google::protobuf::Closure {
public:
    /**
     * @brief Construct a new Rpc Closure object
     *
     * @param function
     */
    explicit RpcClosure(std::function<void()> function);
    explicit RpcClosure() = default;
    RpcClosure(const RpcClosure&&) = delete;
    RpcClosure(RpcClosure&&) = delete;
    RpcClosure& operator=(const RpcClosure&) = delete;
    RpcClosure& operator=(RpcClosure&&) = delete;
    ~RpcClosure() = default;

    /**
     * @brief 重载了 google::protobuf::Closure
     *
     */
    void Run() override;

private:
    std::function<void()> m_function { nullptr };
};
} // namespace rpc
#endif
