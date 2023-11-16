#include <cerrno>
#include <unistd.h>
#include <fmt/format.h>
#include "net/wakeup_fd_event.h"
#include "net/fd_event/fd_event.h"
#include "common/log.h"

namespace rpc {
WakeUpFdEvent::WakeUpFdEvent(int fd) : FdEvent(fd) {}
WakeUpFdEvent::~WakeUpFdEvent() {}
// 相当于触发事件，这里向这个文件描述符里面写入东西，eventloop就会检测到，所以就会唤醒
void WakeUpFdEvent::wakeup() {
    char buf[8] = { 'a' };
    int rt = write(m_fd, buf, 8);
    if (rt != 8) {
        ERROR_LOG(fmt::format("write to wakeup fd less 8 bytes fd {}", m_fd));
    }
    DEBUG_LOG("success read 8 byts");
}
} // namespace rpc