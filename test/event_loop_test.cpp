#include "../src/common/log.hpp"
#include "../src/common/config.hpp"
#include "../src/net/eventloop.hpp"
#include "../src/net/fd_event.hpp"


#include <filesystem>
#include <memory>
#include <mutex>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>

int main()
{

    rpc::Config::set_global_config("/home/lzc/tiny_rpc/conf/rpc.xml");

    rpc::Logger::init_global_logger();
    // rpc::EventLoop* eventloop = new rpc::EventLoop();
    std::unique_ptr<rpc::EventLoop> eventloop_ptr = std::make_unique<rpc::EventLoop>();
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        rpc::ERROR_LOG("listenfd = -1");
        exit(0);
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_port = htons(12310);
    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr.sin_addr);

    int rt = bind(listenfd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (rt != 0) {
        rpc::ERROR_LOG("bind() error");
        exit(1);
    }

    rt = listen(listenfd, 100);
    if (rt != 0) {
        rpc::ERROR_LOG("listend() error");
        exit(1);
    }

    rpc::Fd_Event event(listenfd);
    event.listen(rpc::Fd_Event::TriggerEvent::IN_EVENT, [listenfd]()
    {
        sockaddr_in peer_addr;
        socklen_t addr_len = sizeof(peer_addr);
        memset(&peer_addr, 0, sizeof(peer_addr));
        int clientfd = accept(listenfd, reinterpret_cast<sockaddr*>(&peer_addr), &addr_len);
        rpc::DEBUG_LOG(fmt::format("success get client fd[{}], peer addr: [{}:{}]", clientfd, inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port)));

    });
    eventloop_ptr->add_epoll_event(&event);
    
    eventloop_ptr->loop();

    return 0;
}