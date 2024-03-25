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
    int ans_idx = 0;
    int number_fd = m_io_thread_groups[0]->get_evnetloop_task_size();

    for (size_t i = 1; i < m_io_thread_groups.size(); i++) {
        if (m_io_thread_groups[i]->get_evnetloop_task_size() < number_fd) {
            ans_idx = i;
            number_fd = m_io_thread_groups[i]->get_evnetloop_task_size();
        }
    }

    return m_io_thread_groups[ans_idx];
}

void IOThreadGroup::join() {
    for (size_t i = 0; i < m_io_thread_groups.size(); i++) {
        m_io_thread_groups[i]->join();
    }
}
} // namespace rpc