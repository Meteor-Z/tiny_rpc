#include "common/log.hpp"
#include "common/config.hpp"
#include "net/eventloop.hpp"
#include "net/fd_event.hpp"
#include "net/io_thread/io_thread.hpp"
#include "net/time/time_event.hpp"

#include <cstdio>
#include <filesystem>
#include <fmt/core.h>
#include <memory>
#include <mutex>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>

void test_io_thread() {
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        rpc::utils::ERROR_LOG(fmt::format("listend fd = -1"));
        std::exit(0);
    }   
    
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    
    addr.sin_port = htons(11451);
    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr.sin_addr);
    int rt = bind(listenfd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (rt != 0) {
        rpc::utils::ERROR_LOG(fmt::format("bind error"));
        std::exit(0);
    }

    rt = listen(listenfd, 100);
    if (rt != 0) {
        rpc::utils::ERROR_LOG("listen error");
        std::exit(0);
    }

    rpc::Fd_Event event { listenfd };

    event.listen(rpc::Fd_Event::TriggerEvent::IN_EVENT, [listenfd]() {
        sockaddr_in peer_addr;
        socklen_t addr_len = sizeof(peer_addr);
        memset(&peer_addr, 0, sizeof(peer_addr));
        int client_fd = accept(listenfd, reinterpret_cast<sockaddr*>(&peer_addr), &addr_len);
        // 连接成功
        rpc::utils::DEBUG_LOG(fmt::format("success get client fd[{}], peer addr: [{}:{}]", client_fd, inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port)));
    });
    int i = 0;
    rpc::TimerEvent::s_ptr timer_event_ptr = std::make_shared<rpc::TimerEvent>(1000, true, [&i]() {
        rpc::utils::INFO_LOG(fmt::format("trigger timer event, count = {}", i++));
    });

    rpc::IOThread io_thread; // 会析构
    io_thread.get_eventloop()->add_epoll_event(&event);
    io_thread.get_eventloop()->add_timer_event(timer_event_ptr);
    io_thread.start();
    io_thread.join();
}

int main() {
    rpc::Config::set_global_config("/home/lzc/tiny_rpc/conf/rpc.xml");
    rpc::Logger::init_global_logger();
    std::unique_ptr<rpc::EventLoop> eventloop_ptr = std::make_unique<rpc::EventLoop>();
    test_io_thread();
}