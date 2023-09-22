#include "net/wakeup_fd_event.hpp"
#include "net/fd_event.hpp"
#include <asm-generic/errno-base.h>
#include <cerrno>
#include <unistd.h>
#include "common/log.hpp"
#include <fmt/format.h>
namespace rpc
{
    WakeUPEvent::WakeUPEvent(int fd) : Fd_Event(fd) { }
    WakeUPEvent:: ~WakeUPEvent() { }
    void WakeUPEvent::wakeup()
    {
        char buf[8] = { 'a' };
        int rt = write(m_fd, buf, 8);
        if (rt != 8)
        {
            rpc::utils::ERROR_LOG(fmt::format("write to wakeup fd less 8 bytes fd {}", m_fd));
        }   
        rpc::utils::DEBUG_LOG("success read 8 byts");
    }
}