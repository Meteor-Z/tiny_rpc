/*
相关错误码
*/

#ifndef RPC_COMMON_ERROR_CODE_H
#define RPC_COMMON_ERROR_CODE_H

// what the fuck ? 这个是什么语法
// 参数连接
#ifndef SYS_ERROR_PREFIX
#define SYS_ERROR_PREFIX(val) 1000##val

// 连接时对端关闭
constexpr int ERRPR_PEER_CLOSED = SYS_ERROR_PREFIX(0000);

// 连接失败
constexpr int ERRPR_FAILED_CONNECT = SYS_ERROR_PREFIX(0001);

// 获取回包失败
constexpr int ERROR_FAILED_GET_REPLY = SYS_ERROR_PREFIX(0002);

// 序列化失败
constexpr int ERROR_FAILED_SERIALIZE = SYS_ERROR_PREFIX(0003);

// 反序列化失败
constexpr int ERROR_FAILED_DESERIALIZE = SYS_ERROR_PREFIX(0004);

// 编码失败
constexpr int ERROR_FAILED_ENCODE = SYS_ERROR_PREFIX(0005);

// 解码失败
constexpr int ERROR_FAILED_DECODE = SYS_ERROR_PREFIX(0006);

// rpc 调用超时
constexpr int ERROR_RPC_CALL_TIMEOUT = SYS_ERROR_PREFIX(0007);

// service 调用超时
constexpr int ERROR_SERVICE_NOT_FOUND = SYS_ERROR_PREFIX(0008);

// 找不到method 方法
constexpr int ERROR_METHOD_NOT_FOUND = SYS_ERROR_PREFIX(0009);

// service name 解析失败
constexpr int ERROR_PARSE_SERVICE_NAME = SYS_ERROR_PREFIX(0010);

#endif

#endif