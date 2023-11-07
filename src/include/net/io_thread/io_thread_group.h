/*
IO_Group,讲起进行封装，

*/
#ifndef RPC_NET_IO_THREAD_GROUP_H
#define RPC_NET_IO_THREAD_GROUP_H

#include <memory>
#include <vector>
#include "common/log.h"
#include "net/io_thread/io_thread.h"

namespace rpc {
class IOThreadGroup {
public:
    IOThreadGroup(int size);

    ~IOThreadGroup();

    // 集体开始
    void start();
    
    // 集体结束
    void join();

    // 得到当前线程
    std::shared_ptr<IOThread> get_io_thread();

private:
    int m_size { 0 };                                          // groups的size
    std::vector<std::shared_ptr<IOThread>> m_io_thread_groups; // groups
    int m_idx { -1 };                                          // 得到的idx
};
} // namespace rpc

#endif