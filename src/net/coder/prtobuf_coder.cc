#include <vector>
#include "common/log.h"
#include "common/utils.h"
#include "net/coder/protobuf_coder.h"
#include "net/coder/protobuf_protocol.h"

namespace rpc {

void ProtobufCoder::encode(std::vector<std::shared_ptr<AbstractProtocol>>& messages,
                           std::shared_ptr<TcpBuffer> out_buffer) {}
void ProtobufCoder::decode(std::vector<std::shared_ptr<AbstractProtocol>>& out_messages,
                           std::shared_ptr<TcpBuffer> buffer) {
    // 遍历buffer, 找到PB_START, 然后开始解析，判断是否是PB_END;
    // 0x02 0x03 这样的标识符有可能直接不是这个位置，有可能是数据里面的0x02 0x03
    std::vector<char> tmp = buffer->get_buffer();
    int start_index = buffer->read_index();
    int end_index { -1 };
    int pk_len { 0 };

    for (int i = start_index; i < buffer->wtite_index(); i++) {
        // 如果是开始头
        if (tmp[i] == ProtobufProtocol::s_start) {
            // 网络字节序转化成本地的字节序
            if (i + 1 < buffer->wtite_index()) {
                pk_len = rpc::utils::get_int32_from_netbyte(&tmp[i + 1]);
                DEBUG_LOG("")
            }
        }
    }
}
} // namespace rpc