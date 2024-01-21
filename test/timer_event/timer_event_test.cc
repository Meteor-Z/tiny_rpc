#include "common/log.h"
#include "common/log_config.h"
#include "net/eventloop.h"
#include "net/fd_event/fd_event.h"
#include "net/time/time_event.h"
#include <arpa/inet.h>
#include <cstring>
#include <filesystem>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

int main() {
    rpc::LogConfig::SET_GLOBAL_CONFIG("/home/lzc/tiny_rpc/conf/rpc.xml");

    rpc::Logger::INIT_GLOBAL_LOGGER();

    std::unique_ptr<rpc::EventLoop> eventloop_ptr = std::make_unique<rpc::EventLoop>();
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        ERROR_LOG("listenfd = -1");
        exit(0);
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    int ip = 12345;
    DEBUG_LOG(fmt::format("... ip = {}", ip));

    addr.sin_port = htons(ip);
    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr.sin_addr);

    int bind_is_ok = bind(listenfd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (bind_is_ok != 0) {
        ERROR_LOG("bind() error");
        exit(1);
    }

    int listen_is_ok = listen(listenfd, 100); // 监听这个端口

    if (listen_is_ok != 0) {
        ERROR_LOG("listend() error");
        exit(1);
    }
    // 创建一个描述符文件
    // rpc::FdEvent event(listenfd);
    std::shared_ptr<rpc::FdEvent> event = std::make_shared<rpc::FdEvent>(listenfd);
    event->listen(rpc::FdEvent::TriggerEvent::IN_EVENT, [listenfd]() {
        sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        memset(&client_addr, 0, sizeof(client_addr));
        int clientfd =
            accept(listenfd, reinterpret_cast<sockaddr*>(&client_addr), &addr_len);
        DEBUG_LOG(fmt::format("success get client fd[{}], peer addr: [{}:{}]", clientfd,
                              inet_ntoa(client_addr.sin_addr),
                              ntohs(client_addr.sin_port)));
    });

    eventloop_ptr->add_epoll_event(event);

    int i = 0;
    std::shared_ptr<rpc::TimerEvent> timer_event =
        std::make_shared<rpc::TimerEvent>(1000, true, [&i]() {
            INFO_LOG(fmt::format("triger, timer event, count = {}", i++));
        });
    eventloop_ptr->add_timer_event(timer_event);
    eventloop_ptr->loop();
    return 0;
}