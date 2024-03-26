/*
abstract_coder的实现类
是std::string的实现
*/
#pragma once

#ifndef RPC_NET_TCP_STRING_CODER_H
#define RPC_NET_TCP_STRING_CODER_H

#include <iostream>
#include "net/coder/abstract_coder.h"
#include "net/coder/abstract_protocol.h"
#include "net/coder/string_protocol.h"

namespace rpc {
class StringCoder : public AbstractCoder {
public:
    // 编码
    // ok
    void encode(std::vector<std::shared_ptr<AbstractProtocol>>& messages,
                std::shared_ptr<TcpBuffer> out_buffer) override {
        for (size_t i = 0; i < messages.size(); i++) {
            std::shared_ptr<StringProtocol> msg = std::dynamic_pointer_cast<StringProtocol>(messages[i]);
            std::cout << "hahaha" << ' ' << msg->m_info.c_str() << ' ' << msg->m_info.size() << std::endl;
            out_buffer->write_to_buffer(msg->m_info.c_str(), msg->m_info.size());
        }
    }
    // 解码
    // ok
    void decode(std::vector<std::shared_ptr<AbstractProtocol>>& out_messages,
                std::shared_ptr<TcpBuffer> buffer) override {

        std::vector<char> receive;
        buffer->read_from_buffer(receive, buffer->can_read_bytes_num());

        /// TODO: 待优化
        std::string info;
        // 构建成一个字符串
        for (size_t i = 0; i < receive.size(); i++) {
            info += receive[i];
        }

        // 将得到的信息放到字节流里面。
        std::shared_ptr<StringProtocol> msg = std::make_shared<StringProtocol>();
        msg->m_info = info;
        // msg->set_req_id("123456");
        msg->m_msg_id = "123456";
        out_messages.push_back(msg);
    }

    ~StringCoder() {}
};
} // namespace rpc

#endif