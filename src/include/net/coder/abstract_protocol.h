/**
 * @file abstract_protocol.h
 * @author liuzechen (liuzechen.coder@qq.com)
 * @brief 编辑器吗器
 * @version 0.1
 * @date 2024-03-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
 #pragma once

#ifndef RPC_NET_TCP_ABSTRACT_PROTOCOL_H
#define RPC_NET_TCP_ABSTRACT_PROTOCOL_H

#include <memory>
#include <string>
#include "common/log.h"

namespace rpc {
struct AbstractProtocol : public std::enable_shared_from_this<AbstractProtocol> {
public:
    // std::string get_req_id() const noexcept { return m_msg_id; };
    // void set_req_id(const std::string& req_id) noexcept { m_msg_id = req_id; };
    virtual ~AbstractProtocol() { DEBUG_LOG("~AbstractProtocol"); }

public:
    std::string m_msg_id; // 标识的请求号
};

} // namespace rpc
#endif