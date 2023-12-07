#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <functional>
#include <iostream>
#include <memory>
#include "common/log_config.h"
#include "common/log.h"
#include "net/tcp/ipv4_net_addr.h"
#include "net/tcp/tcp_server.h"
#include "net/tcp/tcp_client.h"
#include "net/coder/string_coder.h"
#include "net/coder/abstract_protocol.h"
#include "net/coder/string_protocol.h"

// void test_tcp_client() {
//     // 1. 调用 connect 连接 server
//     // 2. write 一个字符串
//     // 3. 等待 read 返回结果

//     int fd = socket(AF_INET, SOCK_STREAM, 0);

//     if (fd < 0) {
//         exit(0);
//     }

//     sockaddr_in server_addr {};

//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = htons(1245);
//     inet_aton("127.0.0.1", &server_addr.sin_addr);

//     int rt = connect(fd, reinterpret_cast<sockaddr*>(&server_addr),
//     sizeof(server_addr));

//     std::string message = "hello world!";

//     rt = write(fd, message.c_str(), message.size());

//     DEBUG_LOG(fmt::format("succes write {} bytes, {}", rt, message.c_str()));

//     char buf[100];

//     rt = read(fd, buf, 100);

//     DEBUG_LOG(fmt::format("read succes {} bytes, {}", rt, std::string(buf)));
// }

void test_tcp_client_connect() {
    std::shared_ptr<rpc::IPv4NetAddr> addr =
        std::make_shared<rpc::IPv4NetAddr>("127.0.0.1", 1245);

    rpc::TcpClient client(addr);

    // 测试日志
    client.connect([addr, &client]() {
        DEBUG_LOG(
            fmt::format("Test TCP_Client, connect to {} success", addr->to_string()));
        std::shared_ptr<rpc::StringProtocol> message =
            std::make_shared<rpc::StringProtocol>();


        message->m_info = "hello tiny_rpc";
        message->set_req_id("123456");
        client.write_message(message, [](std::shared_ptr<rpc::AbstractProtocol> done) {
            DEBUG_LOG("send message success");
        });

        client.read_message("123456", [](std::shared_ptr<rpc::AbstractProtocol> done) {
            std::shared_ptr<rpc::StringProtocol> message =
                std::dynamic_pointer_cast<rpc::StringProtocol>(done);
            // 等会要进行修改
            DEBUG_LOG(fmt::format("req_id = {}, response success, info = {}",
                                  message->get_req_id(), message->m_info));
        });

        client.write_message(message, [](std::shared_ptr<rpc::AbstractProtocol> done) {
            DEBUG_LOG("send message 2222222 success");
        });
    });
}

int main() {
    rpc::LogConfig::SET_GLOBAL_CONFIG("/home/lzc/tiny_rpc/conf/rpc.xml");
    rpc::Logger::INIT_GLOBAL_LOGGER();

    // test_tcp_client();
    test_tcp_client_connect();
}