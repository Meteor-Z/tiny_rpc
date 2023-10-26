
#include <memory>
#include <mutex>
#include "net/fd_event/fd_event_group.h"
#include "net/fd_event.hpp"

namespace rpc {
    constexpr double EXPEND_FACTOR = 1.5;
    static std::shared_ptr<FdEventGroup> global_fd_event_group{nullptr};
    FdEventGroup::~FdEventGroup() {
    }

    std::shared_ptr<Fd_Event> FdEventGroup::get_fd_event(int fd) {
        std::lock_guard<std::mutex> lock_guard{m_mutex};

        if (fd < m_fd_event_groups.size()) {
            return m_fd_event_groups[fd];
        }

        int new_size = static_cast<int>(m_size * EXPEND_FACTOR);

        for (int i = m_size; i < new_size; i++) {
            m_fd_event_groups.emplace_back(std::make_shared<Fd_Event>(i));
        }

        m_size = new_size;

        return m_fd_event_groups[fd];
    }
    FdEventGroup::FdEventGroup(int size) :
        m_size(size) {
        for (int i = 0; i < m_size; i++) {
            m_fd_event_groups.emplace_back(std::make_shared<Fd_Event>(i));
        }
    }

    std::shared_ptr<FdEventGroup> FdEventGroup::Get_Fd_Event_Group() {
        if (!global_fd_event_group) {
            global_fd_event_group = std::make_shared<FdEventGroup>(128);
        }

        return global_fd_event_group;
    }
} // namespace rpc