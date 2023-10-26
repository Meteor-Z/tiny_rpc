/**



    read_index              write_index
 ----------------------------------------------
    |                           |
    |                           |
 ---------------------------------------------
*/

#ifndef RPC_NET_TCP_BUFFER_H
#define RPC_NET_TCP_BUFFER_H

#include <string>
#include <vector>
namespace rpc {
class TcpBuffer {
public:
    TcpBuffer(int size);
    ~TcpBuffer();
    int can_read_bytes_num();  // 可写字节数
    int can_write_bytes_num(); // 可写的字节数
    int read_index();
    int wtite_index();
    void read_to_buffer(const char *buffer, int size);
    void read_from_buffer(std::vector<char> &re, int size);
    void adjust_buffer();
    void resize_buffer(int new_size);
    void adjust_read_index(int size);
    void adjust_write_index(int size);

private:
    int m_read_index{0};
    int m_write_index{0};
    int m_size{0};

public:
    std::vector<char> m_buffer{};
};
}; // namespace rpc

#endif