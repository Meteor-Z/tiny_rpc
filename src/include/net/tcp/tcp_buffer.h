/**
 * @file tcp_buffer.h
 * @author liuzechen (liuzechen.coder@qq.com)
 * @brief Tcp_Buffer 数据缓冲
 * @version 0.1
 * @date 2024-03-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef RPC_NET_TCP_BUFFER_H
#define RPC_NET_TCP_BUFFER_H

#include <vector>

namespace rpc {
/**
 * @brief
 * 发送数据放到发送缓冲区里面，等待epoll异步去发送
 * 提高发送效率，多个包合并到一起进行打包。

 *    read_index              write_index
 * ----------------------------------------------
 *    |                           |
 *    |                           |
 * ---------------------------------------------
 */
class TcpBuffer {
public:
    TcpBuffer(int size);
    ~TcpBuffer();

    // 可以读的字节数
    int can_read_bytes_num() const noexcept;

    /**
     * @brief 返回可写的字节数
     *
     * @return int 字节数
     */
    int can_write_bytes_num() const noexcept;

    /**
     * @brief 可读的 indx
     *
     * @return int
     */
    int read_index() const noexcept;

    /**
     * @brief 可写的 index
     *
     * @return int
     */
    int write_index() const noexcept;

    /**
     * @brief 将char写入到 buffer 里面
     *
     * @param buffer 字节数
     * @param size buffer的大小
     */
    void write_to_buffer(const char* buffer, int size);

    /**
     * @brief 读出的字节
     *
     * @param re 要接受的 std::vector<char>
     * @param size 大小
     */
    void read_from_buffer(std::vector<char>& re, int size);

    // 调整，将buffer变成最初始的状态
    // 也就是从头开始读取，移动一下。
    void adjust_buffer();

    /**
     * @brief 扩容
     *
     * @param new_size
     */
    void resize_buffer(int new_size);

    // 将read的index向右边移动 size 距离
    void adjust_read_index(int size);

    // 往右边调整，将 write_indx 从新坐标开始
    void adjust_write_index(int size);

    std::vector<char>& get_buffer() noexcept;

private:
    int m_read_index { 0 };  ///< 读取的位置
    int m_write_index { 0 }; ///< 写入的位置
    int m_size { 0 };        ///< 大小

public:
    std::vector<char> m_buffer {}; ///< 缓冲区
};
}; // namespace rpc

#endif