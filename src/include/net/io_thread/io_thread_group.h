/**
 * @file io_thread_group.h
 * @author liuzechen.coder (liuzechen.coder@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-03-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#ifndef RPC_NET_IO_THREAD_GROUP_H
#define RPC_NET_IO_THREAD_GROUP_H

#include <memory>
#include <vector>
#include "net/io_thread/io_thread.h"

namespace rpc {
/**
 * @brief IO线程的封装
 *
 */
class IOThreadGroup {
public:
    explicit IOThreadGroup(int size);

    IOThreadGroup(const IOThreadGroup&) = delete;
    IOThreadGroup(IOThreadGroup&&) = delete;
    IOThreadGroup& operator=(const IOThreadGroup&) = delete;
    IOThreadGroup& operator=(IOThreadGroup&&) = delete;

    ~IOThreadGroup();

    // 集体开始
    void start();

    // 集体结束
    void join();

    // 得到当前线程
    std::shared_ptr<IOThread> get_io_thread();

private:
    int m_size { 0 };                                          ///< groups的size
    std::vector<std::shared_ptr<IOThread>> m_io_thread_groups; ///< IO线程组
    int m_idx { -1 };                                          ///< 得到的idx
};
} // namespace rpc

#endif