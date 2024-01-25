#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <memory>
#include "fmt/core.h"
#include "common/log.h"
#include "common/utils.h"
#include "net/coder/abstract_protocol.h"
#include "net/coder/protobuf_coder.h"
#include "net/coder/protobuf_protocol.h"

namespace rpc {

void ProtobufCoder::encode(std::vector<std::shared_ptr<AbstractProtocol>>& messages,
                           std::shared_ptr<TcpBuffer> out_buffer) {
    for (auto& itme : messages) {
        // 先将其转化成 ProtobufProtocol 然后进行操作
        std::shared_ptr<ProtobufProtocol> msg =
            std::dynamic_pointer_cast<ProtobufProtocol>(itme);
        int len = 0;
        // 解析这一个包
        const char* buf = encode_protubuf(msg, len);

        if (len != 0 && buf != nullptr) {
            out_buffer->write_to_buffer(buf, len);
        }

        /// TODO:这里要改一下。这里太怪了
        if (buf) {
            std::free((void*)buf);
            buf = nullptr;
        }
    }
}

/// TODO: 改一下这个命名，index 改成idx什么的
void ProtobufCoder::decode(std::vector<std::shared_ptr<AbstractProtocol>>& out_messages,
                           std::shared_ptr<TcpBuffer> buffer) {
    // 遍历buffer, 找到PB_START, 然后开始解析，判断是否是PB_END;
    // 0x02 0x03 这样的标识符有可能直接不是这个位置，有可能是数据里面的0x02 0x03

    while (true) {
        // 遍历 buffer，找到 s_start
        // 解析整包长度，然后找到 s_end;
        std::vector<char> tmp = buffer->m_buffer;
        int start_index = buffer->read_index();
        int end_index { -1 };

        int pk_len { 0 };
        bool parse_success { false };
        int i { 0 };
        /// TODO: 修改一下名字
        for (i = start_index; i < buffer->write_index(); ++i) {
            // 如果是开始的字节序
            if (tmp[i] == ProtobufProtocol::s_start) {
                // 进行转换
                if (i + 1 < buffer->write_index()) {
                    pk_len = rpc::utils::get_int32_from_netbyte(&tmp[i + 1]);
                    DEBUG_LOG(fmt::format("parse success, pk_len = {}", pk_len));

                    // 结束符的索引
                    int j = i + pk_len - 1;
                    if (j >= buffer->write_index()) {
                        continue;
                    }
                    // 如果是结束符，说明有可能解析成功了
                    if (tmp[j] == ProtobufProtocol::s_end) {
                        start_index = i;
                        end_index = j;
                        parse_success = true;
                        break;
                    }
                }
            }
        }

        if (i >= buffer->write_index()) {
            DEBUG_LOG("decode end, read all buffer data");
            return;
        }

        if (parse_success) {
            buffer->adjust_read_index(end_index - start_index + 1);
            std::shared_ptr<ProtobufProtocol> message =
                std::make_shared<ProtobufProtocol>();
            message->m_pk_len = pk_len;

            int msg_id_len_index = start_index + sizeof(char) + sizeof(message->m_pk_len);
            if (msg_id_len_index >= end_index) {
                message->m_parse_success = false;
                ERROR_LOG(
                    fmt::format("parse error, msg_id_len_index[{}] >= end_index[{}]",
                                msg_id_len_index, end_index));
                continue;
            }
            message->m_msg_id_len =
                rpc::utils::get_int32_from_netbyte(&tmp[msg_id_len_index]);
            DEBUG_LOG(fmt::format("parse msg_id_len={}", message->m_msg_id_len));

            int msg_id_index = msg_id_len_index + sizeof(message->m_msg_id_len);

            char msg_id[100] = { 0 };
            memcpy(&msg_id[0], &tmp[msg_id_index], message->m_msg_id_len);
            message->m_msg_id = std::string(msg_id);
            DEBUG_LOG(fmt::format("parse msg_id={}", message->m_msg_id.c_str()));

            int method_name_len_index = msg_id_index + message->m_msg_id_len;
            if (method_name_len_index >= end_index) {
                message->m_parse_success = false;
                ERROR_LOG(
                    fmt::format("parse error, method_name_len_index[{}] >= end_index[{}]",
                                method_name_len_index, end_index));
                continue;
            }
            message->m_method_name_len =
                rpc::utils::get_int32_from_netbyte(&tmp[method_name_len_index]);

            int method_name_index =
                method_name_len_index + sizeof(message->m_method_name_len);
            char method_name[512] = { 0 };
            memcpy(&method_name[0], &tmp[method_name_index], message->m_method_name_len);
            message->m_method_name = std::string(method_name);
            DEBUG_LOG(
                fmt::format("parse method_name={}", message->m_method_name.c_str()));

            int err_code_index = method_name_index + message->m_method_name_len;
            if (err_code_index >= end_index) {
                message->m_parse_success = false;
                ERROR_LOG(fmt::format("parse error, err_code_index[{}] >= end_index[{}]",
                                      err_code_index, end_index));
                continue;
            }
            message->m_err_code =
                rpc::utils::get_int32_from_netbyte(&tmp[err_code_index]);

            int error_info_len_index = err_code_index + sizeof(message->m_err_code);
            if (error_info_len_index >= end_index) {
                message->m_parse_success = false;
                ERROR_LOG(
                    fmt::format("parse error, error_info_len_index[{}] >= end_index[{}]",
                                error_info_len_index, end_index));
                continue;
            }
            message->m_err_info_len =
                rpc::utils::get_int32_from_netbyte(&tmp[error_info_len_index]);

            int err_info_index = error_info_len_index + sizeof(message->m_err_info_len);
            char error_info[512] = { 0 };
            memcpy(&error_info[0], &tmp[err_info_index], message->m_err_info_len);
            message->m_err_info = std::string(error_info);
            DEBUG_LOG(fmt::format("parse error_info={}", message->m_err_info.c_str()));

            int pb_data_len = message->m_pk_len - message->m_method_name_len -
                              message->m_msg_id_len - message->m_err_info_len - 2 - 24;

            int pd_data_index = err_info_index + message->m_err_info_len;
            message->m_pb_data = std::string(&tmp[pd_data_index], pb_data_len);

            // 这里校验和去解析
            message->m_parse_success = true;

            out_messages.push_back(message);
        }
    }

} // namespace rpc

const char* ProtobufCoder::encode_protubuf(std::shared_ptr<ProtobufProtocol>& message,
                                           int& len) {
    if (message->m_msg_id.empty()) {
        message->m_msg_id = "123456789";
    }
    DEBUG_LOG(fmt::format("msg_id = {}", message->m_msg_id));

    int pk_len = 2 + 24 + message->m_msg_id.length() + message->m_method_name.length() +
                 message->m_err_info.length() + message->m_pb_data.length();
    DEBUG_LOG(fmt::format("pk_len = {}", pk_len));

    char* buf = reinterpret_cast<char*>(malloc(pk_len));
    char* tmp = buf;

    *tmp = ProtobufProtocol::s_start;
    tmp++;

    // 转换成网络序
    int32_t pk_len_net = htonl(pk_len);
    memcpy(tmp, &pk_len_net, sizeof(pk_len_net));
    tmp += sizeof(pk_len_net);

    // 信息包
    int msg_id_len = message->m_msg_id.length();
    int32_t msg_id_len_net = htonl(msg_id_len);
    memcpy(tmp, &msg_id_len_net, sizeof(msg_id_len_net));
    tmp += sizeof(msg_id_len_net);

    if (!message->m_msg_id.empty()) {
        memcpy(tmp, &(message->m_msg_id[0]), msg_id_len);
        tmp += msg_id_len;
    }

    // 方法名
    int method_name_len = message->m_method_name.length();
    int32_t method_name_len_net = htonl(method_name_len);
    std::memcpy(tmp, &method_name_len_net, sizeof(method_name_len_net));
    tmp += sizeof(method_name_len_net);

    if (!message->m_method_name.empty()) {
        std::memcpy(tmp, &(message->m_method_name[0]), method_name_len);
        tmp += method_name_len;
    }

    int32_t err_code_net = htonl(message->m_err_code);
    std::memcpy(tmp, &err_code_net, sizeof(err_code_net));
    tmp += sizeof(err_code_net);

    int err_info_len = message->m_err_info.length();
    int32_t err_info_len_net = htonl(err_info_len);
    std::memcpy(tmp, &err_info_len_net, sizeof(err_info_len_net));
    tmp += sizeof(err_info_len_net);

    if (!message->m_err_info.empty()) {
        std::memcpy(tmp, &(message->m_err_info[0]), err_info_len);
        tmp += err_info_len;
    }

    if (!message->m_pb_data.empty()) {
        std::memcpy(tmp, &(message->m_pb_data[0]), message->m_pb_data.length());
        tmp += message->m_pb_data.length();
    }

    int32_t check_sum_net = htonl(1);
    std::memcpy(tmp, &check_sum_net, sizeof(check_sum_net));
    tmp += sizeof(check_sum_net);

    *tmp = ProtobufProtocol::s_end;

    message->m_pk_len = pk_len;
    message->m_msg_id_len = msg_id_len;
    message->m_method_name_len = method_name_len;
    message->m_err_info_len = err_info_len;
    message->m_parse_success = true;
    len = pk_len;

    DEBUG_LOG(fmt::format("endcode message {} success", message->m_msg_id));

    return buf;
}
ProtobufCoder::~ProtobufCoder() {
    DEBUG_LOG("~ProtobufCoder()...");
}
} // namespace rpc