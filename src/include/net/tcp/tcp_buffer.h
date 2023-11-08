/**
TcpBuffer
发送数据放到发送缓冲区里面，等待epoll异步去发送
提高发送效率，多个包合并到一起进行打包。

    read_index              write_index
 ----------------------------------------------
    |                           |
    |                           |
 ---------------------------------------------
*/

#ifndef RPC_NET_TCP_BUFFER_H
#define RPC_NET_TCP_BUFFER_H

#include <chrono>
#include <string>
#include <vector>

namespace rpc {
class TcpBuffer {
public:
    TcpBuffer(int size);
    ~TcpBuffer();

    // 可以读的字节数
    int can_read_bytes_num() const noexcept;

    // 可写的字节数
    int can_write_bytes_num() const noexcept;

    int read_index() const noexcept;

    int wtite_index() const noexcept;

    // 写入到 buffer 里面
    // buffer: 字节数
    // size: buffer的大小
    void write_to_buffer(const char* buffer, int size);

    // 读出字节
    // re: 读入到此 std::vector<char> 里面
    // size: 大小
    void read_from_buffer(std::vector<char>& re, int size);

    // 调整，将buffer变成最初始的状态
    void adjust_buffer();
    
    // 扩容
    void resize_buffer(int new_size);

    // 往右边调整， read_index 从新坐标开始
    void adjust_read_index(int size);
    
    // 往右边调整，将 write_indx 从新坐标开始
    void adjust_write_index(int size);

    std::vector<char>& get_buffer() noexcept;

private:
    int m_read_index { 0 };  // 读取的位置
    int m_write_index { 0 }; // 写入的位置
    int m_size { 0 };        // ？

public:
    std::vector<char> m_buffer {}; // 读入字节
};
}; // namespace rpc

#endif