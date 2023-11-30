/*
abstract_coder的实现类
是std::string的实现
*/

#ifndef RPC_NET_TCP_STRING_CODER_H
#define RPC_NET_TCP_STRING_CODER_H

#include "net/tcp/abstract_coder.h"
namespace rpc {
class StringCoder : public AbstractCoder {

    void encode(std::vector<std::shared_ptr<AbstractProtocol>>& messages,
                std::shared_ptr<TcpBuffer> out_buffer) override;

    void decode(std::vector<std::shared_ptr<AbstractProtocol>>& out_messages,
                std::shared_ptr<TcpBuffer> buffer) override;
};
} // namespace rpc

#endif