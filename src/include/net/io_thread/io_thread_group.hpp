/*
这是一个IO 线程组，将 io_thread 当成一个group,然后进行多线程开搞！

*/
#ifndef RPC_NET_IO_THREAD_GROUP_H
#define RPC_NET_IO_THREAD_GROUP_H

#include "common/log.hpp"
#include "net/io_thread/io_thread.hpp"

#include <memory>
#include <vector>

namespace rpc {
    class IOThreadGroup {
    public:
        IOThreadGroup(int size);
        ~IOThreadGroup();
        void start();
        void join();
        std::shared_ptr<IOThread> get_io_thread();
    private:
        int m_size { 0 };
        std::vector<std::shared_ptr<IOThread>> m_io_thread_groups;
        int m_idx { -1 };
    };
}

#endif