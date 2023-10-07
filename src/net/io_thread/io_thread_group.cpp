#include "net/io_thread/io_thread_group.hpp"
#include "net/io_thread/io_thread.hpp"
#include <optional>

namespace rpc {

 
    IOThreadGroup::IOThreadGroup(int size) : m_size { size } {
        m_io_thread_groups.resize(m_size);

        for (int i = 0; i < size; i++)
        {
            m_io_thread_groups[i] = std::make_shared<IOThread>();
        }
    }

    IOThreadGroup::~IOThreadGroup() {

    }
    
    void IOThreadGroup::start() {
        for (int i = 0; i < m_io_thread_groups.size(); i++) {
            m_io_thread_groups[i]->start();
        }
    }

    std::shared_ptr<IOThread> IOThreadGroup::get_io_thread() {
        if (m_idx == m_io_thread_groups.size() || m_idx == -1) {
            m_idx = 0;
        }
        return m_io_thread_groups[m_idx++];
    }
    void IOThreadGroup::join() {
        for (int i = 0; i < m_io_thread_groups.size(); i++) {
            m_io_thread_groups[i]->join();
        }
    }
}