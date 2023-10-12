#ifndef RPC_NET_WAKEUP_EVENT_H
#define RPC_NET_WAKEUP_EVENT_H

#include "net/fd_event.hpp"

namespace rpc {
    class WakeUPEvent : public Fd_Event {
    public:
        WakeUPEvent(int fd);
        ~WakeUPEvent();
        void wakeup();
    };
}
#endif