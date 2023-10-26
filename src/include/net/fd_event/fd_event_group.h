#ifndef RPC_NET_FD_EVENT_GROUP_H
#define RPC_NET_FD_EVENT_GROUP_H

#include <mutex>
#include <vector>
#include <memory>
#include "net/fd_event/fd_event.h"

namespace rpc {
    class FdEventGroup {
    public:
        FdEventGroup(int size);
        ~FdEventGroup();
        std::shared_ptr<Fd_Event> get_fd_event(int fd);
    public:
        static std::shared_ptr<FdEventGroup> Get_Fd_Event_Group();
    private:
        int m_size{0}; 
        std::vector<std::shared_ptr<Fd_Event>> m_fd_event_groups;
        std::mutex m_mutex;
    };
} // namespace rpc
#endif