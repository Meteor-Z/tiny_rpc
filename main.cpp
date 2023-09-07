#include "src/common/log.hpp"
#include "src/common/config.hpp"
#include "src/net/eventloop.hpp"
#include "src/net/fd_event.hpp"


#include <filesystem>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>

int main()
{
    // 这个是设置log的
    rpc::Config::set_global_config("/home/lzc/tiny_rpc/conf/rpc.xml");
    rpc::Logger::init_global_logger();
    
    rpc::EventLoop* eventloop = new rpc::EventLoop();
    int listend_fd = socket(AF_INET,SOCK_STREAM, 0);
    if (listend_fd == -1)
    {
        rpc::ERROR_LOG("listendfd = -1");
        std::exit(0);
    }
    
    sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_port = htons(1234);
    server_addr.sin_family = AF_INET;
    
    inet_aton("127.0.0.1", &server_addr.sin_addr);
    
    int flag = bind(listend_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
    
    if (flag != 0)
    {
        rpc::ERROR_LOG("bind() error");
        exit(1);
    }
    flag = listen(listend_fd, 100);
    
    rpc::Fd_Event event(listend_fd);
    
    event.listen(rpc::Fd_Event::TriggerEvent::IN_EVENT, [listend_fd]()
    {
        sockaddr_in peer_addr;
        socklen_t addr_len = 0;
        memset(&peer_addr, 0, sizeof(peer_addr));
        int client_fd = accept(listend_fd, reinterpret_cast<sockaddr*>(&peer_addr), &addr_len);
        inet_ntoa(peer_addr.sin_addr);
        rpc::DEBUG_LOG(fmt::format("success get client [{}:{}]", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port)));
        
    });
    eventloop->add_epoll_event(&event);
    eventloop->loop();



    

}