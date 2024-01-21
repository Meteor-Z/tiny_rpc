#include "common/log.h"
#include "common/log_config.h"
#include "net/eventloop.h"
#include "net/fd_event/fd_event.h"
#include "net/io_thread/io_thread.h"
#include "net/io_thread/io_thread_group.h"
#include "net/time/time_event.h"
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fmt/core.h>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

void test_io_thread() {
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        ERROR_LOG(fmt::format("listend fd = -1"));
        std::exit(0);
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_port = htons(11451);
    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr.sin_addr);

    int rt = bind(listenfd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (rt != 0) {
        ERROR_LOG(fmt::format("bind error"));
        std::exit(0);
    }

    rt = listen(listenfd, 100);
    if (rt != 0) {
        ERROR_LOG("listen error");
        std::exit(0);
    }

    // rpc::FdEvent event { listenfd };
    std::shared_ptr<rpc::FdEvent> event = std::make_shared<rpc::FdEvent>(listenfd);

    event->listen(rpc::FdEvent::TriggerEvent::IN_EVENT, [listenfd]() {
        sockaddr_in peer_addr;
        socklen_t addr_len = sizeof(peer_addr);
        memset(&peer_addr, 0, sizeof(peer_addr));
        int client_fd =
            accept(listenfd, reinterpret_cast<sockaddr*>(&peer_addr), &addr_len);
        // 连接成功
        DEBUG_LOG(fmt::format("success get client fd[{}], peer addr: [{}:{}]", client_fd,
                              inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port)));
    });

    int i = 0;
    std::shared_ptr<rpc::TimerEvent> timer_event_ptr =
        std::make_shared<rpc::TimerEvent>(1000, true, [&i]() {
            INFO_LOG(fmt::format("trigger timer event, count = {}", i++));
        });

    // rpc::IOThread io_thread; // 会析构
    // io_thread.get_eventloop()->add_epoll_event(&event);
    // io_thread.get_eventloop()->add_timer_event(timer_event_ptr);
    // io_thread.start();
    // io_thread.join();

    std::shared_ptr<rpc::IOThreadGroup> io_thread_group_ptr =
        std::make_shared<rpc::IOThreadGroup>(2);
    std::shared_ptr<rpc::IOThread> io_thread_ptr = io_thread_group_ptr->get_io_thread();
    io_thread_ptr->get_eventloop()->add_epoll_event(event);
    io_thread_ptr->get_eventloop()->add_timer_event(timer_event_ptr);

    std::shared_ptr<rpc::IOThread> io_thread_ptr_2 = io_thread_group_ptr->get_io_thread();
    io_thread_ptr_2->get_eventloop()->add_timer_event(timer_event_ptr);

    io_thread_group_ptr->start();
    io_thread_group_ptr->join();
}

int main() {
    rpc::LogConfig::SET_GLOBAL_CONFIG("/home/lzc/tiny_rpc/conf/rpc.xml");
    rpc::Logger::INIT_GLOBAL_LOGGER();
    std::unique_ptr<rpc::EventLoop> eventloop_ptr = std::make_unique<rpc::EventLoop>();
    test_io_thread();
}