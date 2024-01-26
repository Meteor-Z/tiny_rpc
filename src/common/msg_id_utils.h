#include <cstdint>
#include <fcntl.h>
#include <unistd.h>
#include "common/msg_id_utils.h"
#include "common/log.h"

namespace rpc {
namespace utils {
static constexpr int32_t g_msg_id_length { 20 }; ///< msg_id 的长度

static thread_local std::string t_cur_msg_id_number {}; ///< 当前 msg_id的号码

static thread_local std::string t_max_msg_id_number {}; ///< 最大 msg_id 的 数量

static int g_random_fd { -1 };

// what ?
std::string MsgIdUtils::gen_msg_id() {
    // 是空的，或者说是等于最大值了
    if (t_cur_msg_id_number.empty() || t_cur_msg_id_number == t_max_msg_id_number) {
        if (g_random_fd == -1) {
            // 打开这个随机文件
            g_random_fd = open("/dev/urandom", O_RDONLY);
        }

        std::string res(g_msg_id_length, 0);

        // 从这里面读入这些随机数字，如果不等于这个数字，就是 error
        if ((read(g_random_fd, &res[0], g_msg_id_length)) != g_msg_id_length) {
            ERROR_LOG("read /dev/urandom error");
            return "";
        }

        for (int i = 0; i < g_msg_id_length; i++) {
            uint8_t val = static_cast<uint8_t>(res[i]) % 10;
            res[i] = val + '0';
            t_max_msg_id_number += "9";
        }
        t_cur_msg_id_number = res;
    } else {
        int i = t_cur_msg_id_number.size() - 1;
        while (i >= 0 && t_cur_msg_id_number[i] == '9') {
            i--;
        }
        if (i >= 0) {
            t_cur_msg_id_number[i] += 1;
            for (size_t j = i + 1; j < t_max_msg_id_number.size(); j++) {
                t_cur_msg_id_number[j] = '0';
            }
        }
    }
    return t_cur_msg_id_number;
}

} // namespace utils
} // namespace rpc