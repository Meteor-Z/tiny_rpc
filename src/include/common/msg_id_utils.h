/**
 * @file msg_id_utils.h
 * @author lzc (liuzechen@qq.com)
 * @brief msg_id的工具
 * @version 0.1
 * @date 2024-01-26
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef RPC_COMMON_MSG_ID_UTILS_H
#define RPC_NET_RPC_RPC_CHANNEL_H

#include <string>

/// TODO: 这里太麻烦了，要改一下

namespace rpc {
namespace utils {
class MsgIdUtils {
public:
    /**
     * @brief 生成msg_id
     *
     * @return std::string msg_id
     */
    static std::string gen_msg_id();
};
} // namespace utils
} // namespace rpc

#endif