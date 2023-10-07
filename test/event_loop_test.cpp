#include "common/log.hpp"
#include "common/config.hpp"
#include "net/eventloop.hpp"
#include "net/fd_event.hpp"


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

    // 设置日志库
    rpc::Config::set_global_config("/home/lzc/tiny_rpc/conf/rpc.xml");

    rpc::Logger::init_global_logger();
    std::unique_ptr<rpc::EventLoop> eventloop_ptr = std::make_unique<rpc::EventLoop>();
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) 
    {
        rpc::utils::ERROR_LOG("listenfd = -1 创建套接字失败");
        exit(0);
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    int ip = 12345;
    rpc::utils::DEBUG_LOG(fmt::format("... ip = {}", ip));
    
    addr.sin_port = htons(ip);
    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr.sin_addr);

    int bind_is_ok = bind(listenfd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    
    if (bind_is_ok != 0) 
    {
        rpc::utils::ERROR_LOG("bind() error");
        exit(1);
    }

    int listen_is_ok = listen(listenfd, 100); // 监听这个端口
    
    if (listen_is_ok != 0) {
        rpc::utils::ERROR_LOG("listend() error");
        exit(1);
    }

    rpc::Fd_Event event(listenfd); // 创建一个描述符文件
    event.listen(rpc::Fd_Event::TriggerEvent::IN_EVENT, [listenfd]()
    {
        sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        memset(&client_addr, 0, sizeof(client_addr));
        int clientfd = accept(listenfd, reinterpret_cast<sockaddr*>(&client_addr), &addr_len);
        rpc::utils::DEBUG_LOG(fmt::format("success get client fd[{}], peer addr: [{}:{}]", clientfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port)));
    });

    eventloop_ptr->add_epoll_event(&event);
    
    eventloop_ptr->loop();

    return 0;
}