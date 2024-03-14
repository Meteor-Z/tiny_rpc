/**
 * @file wakeup_fd_event.h
 * @author liuzechen (liuzechen.coder@qq.com)
 * @brief 对唤醒描述符进行进一步的封装
 * @version 0.1
 * @date 2024-03-14
 * @note 大致原理是：如果想要唤醒eventloop，就往里面加入一个事件，然后让事件进行回应，这里是读取了1个字节，也就是八位
 * @copyright Copyright (c) 2024
 *
 */

#ifndef RPC_NET_WAKEUP_EVENT_H
#define RPC_NET_WAKEUP_EVENT_H

#include "net/fd_event/fd_event.h"

namespace rpc {
class WakeUpFdEvent : public FdEvent {
public:
    /**
     * @brief Construct a new Wake Up Fd Event object
     *
     * @param fd
     */
    WakeUpFdEvent(int fd);
    ~WakeUpFdEvent();
    /**
     * @brief 触发事件
     *
     */
    void wakeup();
};
} // namespace rpc
#endif