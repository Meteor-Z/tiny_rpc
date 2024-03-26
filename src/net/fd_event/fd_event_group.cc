#include <memory>
#include <mutex>
#include "net/fd_event/fd_event_group.h"
#include "common/log.h"
#include "net//fd_event/fd_event.h"

namespace rpc {
constexpr double FD_EVENT_GROUP_EXPEND_FACTOR { 1.5f }; ///< 扩大因素

static std::shared_ptr<FdEventGroup> g_fd_event_group { nullptr };

FdEventGroup::~FdEventGroup() { INFO_LOG("~FdEventGroup::~FdEventGroup()"); }

FdEventGroup::FdEventGroup(int size) : m_size(size) {
    for (int i = 0; i < m_size; i++) {
        m_fd_event_groups.emplace_back(std::make_shared<FdEvent>(i));
    }
}
// 如果有这个fd,那么就直接返回，如果没有，那么就创建，然后返回
std::shared_ptr<FdEvent> FdEventGroup::get_fd_event(int fd) {
    std::lock_guard<std::mutex> lock_guard { m_mutex };

    if (static_cast<size_t>(fd) < m_fd_event_groups.size()) {
        return m_fd_event_groups[fd];
    }

    int new_size = static_cast<int>(m_size * FD_EVENT_GROUP_EXPEND_FACTOR);

    for (int i = m_size; i < new_size; i++) {
        m_fd_event_groups.emplace_back(std::make_shared<FdEvent>(i));
    }

    m_size = new_size;

    return m_fd_event_groups[fd];
}

std::shared_ptr<FdEventGroup> FdEventGroup::Get_Fd_Event_Group() {
    if (!g_fd_event_group) {
        g_fd_event_group = std::make_shared<FdEventGroup>(128);
    }

    return g_fd_event_group;
}
} // namespace rpc