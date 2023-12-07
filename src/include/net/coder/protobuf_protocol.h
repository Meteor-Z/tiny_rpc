/**
 * @file protobuf_protocol.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-12-07
 *
 * @copyright Copyright (c) 2023
 */

/*
protobuf字节序

使用大端存储：
开始符 - 整包长度（四字节，包含开始符和结束符） - reqid长度 - reqid -
方法名长度 - 方法名 - 错误码 - 错误信息长度 - 错误信息 - protobuf序列化数据 -
校验码（我觉得应该使用md5）-结束符


*/

#ifndef RPC_NET_CODER_PROTOBUF_PROTOCOL_H
#define RPC_NET_CODER_PROTOBUF_PROTOCOL_H

#include <cstdint>
#include <string>
#include "net/coder/abstract_protocol.h"

namespace rpc {
struct ProtobufProtocol : public AbstractProtocol {
public:
    static char s_start; // 开始符
    static char s_end;   // 结束符
public:
    int32_t m_pk_len { 0 };       // 整包的长度
    /*std::string m_req_id {};*/  // 请求id (继承于父类)
    int32_t m_req_id_len { 0 };   // 请求id
    int32_t m_method_len { 0 };   // 方法名长度
    std::string m_method_name {}; // 方法名
    int32_t m_err_code { 0 };     // 错误信息
    std::string m_err_info {};    // 错误信息
    std::string m_pb_data {};     // 序列化数据
    int32_t m_check_sum { -1 };   // 校验和
};
} // namespace rpc

#endif