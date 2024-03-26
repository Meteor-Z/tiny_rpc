/**
 * @file fd_event_group.h
 * @author lzc (liuzechen.coder@qq.com)
 * @brief 对FdEvent的再次封装 
 * @version 0.1
 * @date 2024-03-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#ifndef RPC_NET_FD_EVENT_GROUP_H
#define RPC_NET_FD_EVENT_GROUP_H

#include <mutex>
#include <vector>
#include <memory>
#include "net/fd_event/fd_event.h"

namespace rpc {
/**
 * @brief 处理事件的Group，默认开启4个线程
 * 
 */
class FdEventGroup {
public:
    explicit FdEventGroup(int size);

    ~FdEventGroup();
    
    FdEventGroup(const FdEventGroup&) = delete;
    FdEventGroup(FdEventGroup&&) = delete;
    FdEventGroup& operator=(const FdEventGroup&) = delete;
    FdEventGroup& operator=(FdEventGroup&&) = delete;    



    /**
     * @brief Get the fd event object
     * 
     * @param fd 
     * @return std::shared_ptr<FdEvent> 文件描述符
     */
    std::shared_ptr<FdEvent> get_fd_event(int fd);

public:
    // 得到全局的 Fd_Event_Group
    static std::shared_ptr<FdEventGroup> Get_Fd_Event_Group();

private:
    int m_size { 0 };                                        // 大小
    std::vector<std::shared_ptr<FdEvent>> m_fd_event_groups; // fd_event的群
    std::mutex m_mutex;                                      // 锁
};
} // namespace rpc
#endif