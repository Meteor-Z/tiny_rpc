#include <cstddef>
#include "net/io_thread/io_thread_group.h"
#include "common/log.h"
#include "net/io_thread/io_thread.h"

namespace rpc {
IOThreadGroup::IOThreadGroup(int size) : m_size { size } {
    m_io_thread_groups.resize(m_size);

    for (int i = 0; i < size; i++) {
        m_io_thread_groups[i] = std::make_shared<IOThread>();
    }
}

IOThreadGroup::~IOThreadGroup() { INFO_LOG("~IOThreadGroup()"); }

void IOThreadGroup::start() {
    for (size_t i = 0; i < m_io_thread_groups.size(); i++) {
        m_io_thread_groups[i]->start();
    }
}

std::shared_ptr<IOThread> IOThreadGroup::get_io_thread() {
    if (static_cast<size_t>(m_idx) == m_io_thread_groups.size() || m_idx == -1) {
        m_idx = 0;
    }

    return m_io_thread_groups[m_idx++];
}

void IOThreadGroup::join() {
    for (size_t i = 0; i < m_io_thread_groups.size(); i++) {
        m_io_thread_groups[i]->join();
    }
}
} // namespace rpc