/*
abstract_coder的实现类
是std::string的实现
*/

#ifndef RPC_NET_TCP_STRING_CODER_H
#define RPC_NET_TCP_STRING_CODER_H

#include "net/tcp/abstract_coder.h"
#include "net/tcp/abstract_protocol.h"
#include "net/tcp/string_protocol.h"
namespace rpc {
class StringCoder : public AbstractCoder {
public:
    // 编码
    void encode(std::vector<std::shared_ptr<AbstractProtocol>>& messages,
                std::shared_ptr<TcpBuffer> out_buffer) override {
        for (int i = 0; i < messages.size(); i++) {
            std::shared_ptr<StringProtocol> message =
                std::dynamic_pointer_cast<StringProtocol>(messages[i]);
            out_buffer->write_to_buffer(message->m_info.c_str(), message->m_info.size());
        }
    }
    // 解码
    void decode(std::vector<std::shared_ptr<AbstractProtocol>>& out_messages,
                std::shared_ptr<TcpBuffer> buffer) override {

        std::vector<char> receive;
        buffer->read_from_buffer(receive, buffer->can_read_bytes_num());

        /// TODO: 待优化
        std::string info;
        // 构建成一个字符串
        for (const auto& item : receive) {
            info += info;
        }

        // 将得到的信息放到字节流里面。
        std::shared_ptr<StringProtocol> message = std::make_shared<StringProtocol>();
        message->m_info = info;
        out_messages.push_back(message);
    }

    ~StringCoder() {}
};
} // namespace rpc

#endif