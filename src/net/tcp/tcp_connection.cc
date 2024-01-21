#include <cerrno>
#include <cmath>
#include <cstddef>
#include <google/protobuf/descriptor.h>
#include <memory>
#include <sys/socket.h>
#include <type_traits>
#include <unistd.h>
#include <utility>
#include <vector>
#include <queue>
#include "fmt/core.h"
#include "common/log.h"
#include "net/eventloop.h"
#include "net/tcp/ipv4_net_addr.h"
#include "net/tcp/tcp_connection.h"
#include "net/tcp/tcp_buffer.h"
#include "net/fd_event/fd_event.h"
#include "net/fd_event/fd_event_group.h"
#include "net/time/time_event.h"
#include "net/coder/string_coder.h"
#include "net/coder/string_protocol.h"
#include "net/coder/protobuf_protocol.h"
#include "net/coder/abstract_protocol.h"
#include "net/coder/protobuf_coder.h"

namespace rpc {
TcpConnection::TcpConnection(
    std::shared_ptr<EventLoop> event_loop, int fd, int buffer_size,
    std::shared_ptr<IPv4NetAddr> peer_addr,
    TcpConnectionType type /* = TcpConnectionType::TcpConnectionByServer */)
    : m_peer_addr(peer_addr), m_event_loop(event_loop), m_state(TcpState::NotConnected),
      m_fd(fd), m_connection_type(type) {

    // 初始化buffer的大小
    m_in_buffer = std::make_shared<TcpBuffer>(buffer_size);
    m_out_buffer = std::make_shared<TcpBuffer>(buffer_size);

    m_fd_event = FdEventGroup::Get_Fd_Event_Group()->get_fd_event(fd);

    // 设置称非堵塞
    m_fd_event->set_no_block();

    // /// TODO:(这里好像可以优化一下)
    // m_fd_event->listen(FdEvent::TriggerEvent::IN_EVENT,
    //                    std::bind(&TcpConnection::on_read, this));

    // // m_event_loop->get_eventloop()->add_epoll_event(m_fd_event.get());
    // m_event_loop->add_epoll_event(m_fd_event.get());

    m_coder = std::make_shared<ProtobufCoder>();

    // 必须是server端才能进行监听
    if (m_connection_type == TcpConnectionType::TcpConnectionByServer) {
        listen_read();
        // 只有作为 Server 的时候才会使用
        m_dispatcher = std::make_shared<RpcDispatcher>();
    }
}

TcpConnection::~TcpConnection() { DEBUG_LOG("~TcpConnection"); }

void TcpConnection::set_state(const TcpConnection::TcpState& state) { m_state = state; }

TcpConnection::TcpState TcpConnection::get_state() const noexcept { return m_state; }

void TcpConnection::on_read() {
    // 如果不是连接中
    if (m_state != TcpState::Connected) {
        INFO_LOG(fmt::format("client has already disconnected, addr = {}, clientfd {}",
                             m_peer_addr->to_string(), m_fd_event->get_fd()));
        return;
    }

    // 是否读完？
    bool is_read_all { false };
    bool is_close { false };

    while (!is_read_all) {
        //
        if (m_in_buffer->can_write_bytes_num() == 0) {
            fmt::println("hello");
            m_in_buffer->resize_buffer(2 * m_in_buffer->get_buffer().size());
        }
        int read_count = m_in_buffer->can_write_bytes_num();
        int write_index = m_in_buffer->write_index();

        int rt = ::read(m_fd, &(m_in_buffer->get_buffer()[write_index]), read_count);

        INFO_LOG(fmt::format("success read {} bytes fron {}, client fd = {}", rt,
                             m_peer_addr->to_string(), m_fd));

        // 读成功了！进行调整
        if (rt > 0) {
            m_in_buffer->adjust_write_index(rt);

            // 还有数据没有读完
            if (rt == read_count) {
                continue;
            } else if (rt < read_count) {
                is_read_all = true;
                break;
            }

        } else if (rt == 0) {
            is_close = true;
            break;
        } else if (rt == -1 && errno == EAGAIN) {
            is_read_all = true;
            break;
        }
    }

    // 处理关闭连接
    if (is_close) {
        DEBUG_LOG(fmt::format("peer closed, peer addr = {}, client_fd = {}",
                              m_peer_addr->to_string(), m_fd));
        clear();
        return;
    }

    if (!is_read_all) {
        INFO_LOG("not read all data");
    }
    // 读完就开始进行rpc解析
    excute();
}

// ok
// 将RPC请求执行业务逻辑，获取RPC相应，再将RPC响应发送回去
void TcpConnection::excute() {
    // 这个是服务端的做法
    if (m_connection_type == TcpConnectionType::TcpConnectionByServer) {
        // // 先将数据读取出来
        // std::vector<char> temp;
        // int size = m_in_buffer->can_read_bytes_num();
        // temp.resize(size);
        // m_in_buffer->read_from_buffer(temp, size);

        // std::string message;
        // for (size_t i = 0; i < temp.size(); i++) {
        //     message += temp[i];
        // }
        std::vector<std::shared_ptr<AbstractProtocol>> result;
        std::vector<std::shared_ptr<AbstractProtocol>> replay_result;

        m_coder->decode(result, m_in_buffer);

        for (size_t i = 0; i < result.size(); i++) {
            DEBUG_LOG(fmt::format("success get request from client {}, info [{}]",
                                  result[i]->m_msg_id, m_peer_addr->to_string()));

            std::shared_ptr<ProtobufProtocol> message =
                std::make_shared<ProtobufProtocol>();
            // message->m_pb_data = "hello txt....";
            // message->m_msg_id = result[i]->m_msg_id;
            // 以前是随便写的，现在换成 dispatcher的
            m_dispatcher->dispatcher(result[i], message);
            replay_result.emplace_back(message);
        }

        m_coder->encode(replay_result, m_out_buffer);

        listen_write();
        // INFO_LOG(fmt::format("success get request from client {}, info [{}]",
        //                      m_peer_addr->to_string(), message));
        // // 写入到 buffer 里面
        // m_out_buffer->write_to_buffer(message.c_str(), message.size());

        // listen_write();

        // m_fd_event->listen(FdEvent::TriggerEvent::OUT_EVENT,
        //                    std::bind(&TcpConnection::on_write, this));
        // m_event_loop->add_epoll_event(m_fd_event.get());

    } else {
        // 这个是客户端的做法
        // 解码，然后从buffer里面decoder相关message对象，判断是否有req_id相等，如果相等，那么就执行回调
        std::vector<std::shared_ptr<AbstractProtocol>> result;

        // 解码，然后结果是放在了 result 上面了
        m_coder->decode(result, m_in_buffer);

        for (size_t i = 0; i < result.size(); i++) {
            // std::string request_id = result[i]->get_req_id();
            std::string request_id = result[i]->m_msg_id;
            auto it = m_read_dones.find(request_id);
            if (it != m_read_dones.end()) {
                it->second(result[i]->shared_from_this());
            }
        }
    }
}

void TcpConnection::shutdown() {
    if (m_state == TcpConnection::TcpState::Closed ||
        m_state == TcpConnection::TcpState::NotConnected) {
        return;
    }

    // 在半关闭
    m_state = TcpConnection::TcpState::HalfClosing;
    // 触发四次挥手 fin报文
    ::shutdown(m_fd, SHUT_RDWR);
}
void TcpConnection::clear() {
    if (m_state == TcpConnection::TcpState::Closed) {
        return;
    }

    // 取消监听读和写事件
    m_fd_event->cancel(FdEvent::TriggerEvent::IN_EVENT);
    m_fd_event->cancel(FdEvent::TriggerEvent::OUT_EVENT);

    // 去除套接字
    // m_io_thread->get_eventloop()->delete_epoll_event(m_fd_event.get());

    // 去除套接字
    m_event_loop->delete_epoll_event(m_fd_event.get());

    // 这时候才会正式关闭
    m_state = TcpConnection::TcpState::Closed;
}

void TcpConnection::listen_write() {
    m_fd_event->listen(FdEvent::TriggerEvent::OUT_EVENT,
                       std::bind(&TcpConnection::on_write, this));
    m_event_loop->add_epoll_event(m_fd_event.get());
}

// ok
void TcpConnection::listen_read() {
    m_fd_event->listen(FdEvent::TriggerEvent::IN_EVENT,
                       std::bind(&TcpConnection::on_read, this));
    m_event_loop->add_epoll_event(m_fd_event.get());
}

void TcpConnection::push_send_message(
    std::shared_ptr<AbstractProtocol> message,
    std::function<void(std::shared_ptr<AbstractProtocol>)> done) {
    m_write_dones.push_back(std::make_pair(message, done));
}

void TcpConnection::push_read_message(
    const std::string& req_id,
    std::function<void(std::shared_ptr<AbstractProtocol>)> done) {
    //    m_read_dones[req_id] = done;
    // ？
    m_read_dones.insert(std::make_pair(req_id, done));
}

void TcpConnection::set_connection_type(TcpConnectionType type) noexcept {
    m_connection_type = type;
}

void TcpConnection::on_write() {
    // 将当前 out_buffer 发送到到 client

    // 如果已经关闭了
    if (m_state != TcpState::Connected) {
        ERROR_LOG(fmt::format(
            "on_write() error, already disconnected, addr = {}, client fd = {}",
            m_peer_addr->to_string(), m_fd));
        return;
    }

    // 如果是客户端
    if (m_connection_type == TcpConnectionType::TcpConnectionByClient) {
        // 客户端的任务
        // 1. 将 message encode 编码 到字节流里面
        // 2. 将字节流里面的代码输到buffer里面，然后
        std::vector<std::shared_ptr<AbstractProtocol>> messages;

        for (size_t i = 0; i < m_write_dones.size(); i++) {
            messages.push_back(m_write_dones[i].first);
        }
        // 编码之后将所有的信息放到发送缓冲区里面
        m_coder->encode(messages, m_out_buffer);
    }

    bool is_write_all = false;
    // 一直发送，直到发送完
    while (true) {
        if (m_out_buffer->can_read_bytes_num() == 0) {
            DEBUG_LOG(fmt::format("no data need to send to client {}",
                                  m_peer_addr->to_string()));
            is_write_all = true;
            break;
        }
        int size = m_out_buffer->can_read_bytes_num();
        int read_index = m_out_buffer->read_index();

        // 发送
        int result = ::write(m_fd, &(m_out_buffer->get_buffer()[read_index]), size);

        // 发送完了
        if (result >= size) {
            DEBUG_LOG(fmt::format("no data need to client {}", m_peer_addr->to_string()));
            is_write_all = true;
            break;
        }
        // 缓冲区已经满了
        if (result == -1 && errno == EAGAIN) {
            ERROR_LOG(fmt::format("write data error, errno = EAGIN and result = -1"));
            break;
        }
    }

    if (is_write_all) {
        m_fd_event->cancel(FdEvent::TriggerEvent::OUT_EVENT);
        // m_io_thread->get_eventloop()->add_epoll_event(m_fd_event.get());
        m_event_loop->add_epoll_event(m_fd_event.get());
    }

    if (m_connection_type == TcpConnectionType::TcpConnectionByClient) {
        for (size_t i = 0; i < m_write_dones.size(); i++) {
            m_write_dones[i].second(m_write_dones[i].first);
        }
        m_write_dones.clear();
    }
}

} // namespace rpc