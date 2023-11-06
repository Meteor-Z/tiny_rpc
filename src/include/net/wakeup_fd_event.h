/*
唤醒事件，如果想要唤醒eventloop,就往里面加入事件，然后事件做出回应，eventloop就会`epoll_wait`，然后相当于唤醒了

*/
#ifndef RPC_NET_WAKEUP_EVENT_H
#define RPC_NET_WAKEUP_EVENT_H

#include "net/fd_event/fd_event.h"

namespace rpc {
    class WakeUPEvent : public FdEvent {
    public:
        WakeUPEvent(int fd);
        ~WakeUPEvent();
        void wakeup();
    };
}
#endif