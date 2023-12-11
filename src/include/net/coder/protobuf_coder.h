/*
protobuf 编解码器



*/
#ifndef RPC_NET_CODER_PROTOBUF_CODER_H
#define RPC_NET_CODER_PROTOBUF_CODER_H

#include "net/coder/abstract_coder.h"
#include "net/coder/abstract_protocol.h"
#include "net/coder/protobuf_protocol.h"

namespace rpc {
class ProtobufCoder : public AbstractCoder {
public:
    // 解码, 将对象转化成字节流，写入到buff里面
    void encode(std::vector<std::shared_ptr<AbstractProtocol>>& messages,
                std::shared_ptr<TcpBuffer> out_buffer) override;
    // 解码， 将buffer里面的字节流转化成message对象
    void decode(std::vector<std::shared_ptr<AbstractProtocol>>& out_messages,
                std::shared_ptr<TcpBuffer> buffer) override;
    ~ProtobufCoder();

private:
    //  给定一个protubuf，然后得到的字节流
    // 这里必须要是int& 需要传参数
    const char* encode_protubuf(std::shared_ptr<ProtobufProtocol>& message, int& len);
};
} // namespace rpc
#endif