/**
 * @file error_code.h
 * @author lzc (liuzechen.coder@qq.com)
 * @brief 里面全部都是错误信息的
 * @version 0.1
 * @date 2024-01-21
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef RPC_COMMON_ERROR_CODE_H
#define RPC_COMMON_ERROR_CODE_H

/**
 * @brief 错误信息的前缀
 *
 */
#ifndef SYS_ERROR_PREFIX
#define SYS_ERROR_PREFIX(xx) 1000##xx
#endif

constexpr int ERROR_PEER_CLOSE = SYS_ERROR_PREFIX(0000);       ///< 连接时对端关闭
constexpr int ERROR_FAILED_CONNECT = SYS_ERROR_PREFIX(0001);   ///< 连接失败
constexpr int ERROR_FAILED_GET_REPLY = SYS_ERROR_PREFIX(0002); ///< 获取回包失败
constexpr int ERROR_FAILED_DESERIALIZE = SYS_ERROR_PREFIX(0003); ///< 反序列化失败
constexpr int ERROR_FAILED_SERIALIZE = SYS_ERROR_PREFIX(0004);   ///< 序列化错误

constexpr int ERROR_FAILED_ENCODE = SYS_ERROR_PREFIX(0005); ///< encode failed
constexpr int ERROR_FAILED_DECODE = SYS_ERROR_PREFIX(0006); ///< decode failed

constexpr int ERROR_RPC_CALL_TIMEOUT = SYS_ERROR_PREFIX(0007); ///< rpc调用超时

constexpr int ERROR_SERVICE_NOT_FOUND = SYS_ERROR_PREFIX(0008); ///< service 不存在
constexpr int ERROR_METHOD_NOT_FOUND = SYS_ERROR_PREFIX(0009);  ///< method 不存在
constexpr int ERROR_PARSE_SERVICE_NAME =
    SYS_ERROR_PREFIX(0010); ///< service name 解析失败

constexpr int ERROR_RPC_CHANNEL_NOT_INIT =
    SYS_ERROR_PREFIX(0011); ///< rpc channel 初始化失败

#endif