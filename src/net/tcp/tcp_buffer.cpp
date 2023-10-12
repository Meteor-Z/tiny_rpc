#include "net/tcp/tcp_buffer.hpp"
#include "common/log.hpp"
#include <cstring>
#include <memory_resource>
#include <vector>
#include <fmt/format.h>

namespace rpc {
    
    TcpBuffer::TcpBuffer(int size) { m_buffer.resize(size); }

    TcpBuffer:: ~TcpBuffer() {

    }

    int TcpBuffer:: can_read_bytes_num() { return m_write_index - m_read_index; }

    int TcpBuffer::can_write_bytes_num() { return m_buffer.size() - m_write_index; }

    int TcpBuffer::read_index() { return m_read_index; }

    int TcpBuffer::wtite_index() { return m_write_index; }

    void TcpBuffer::read_to_buffer(const char* buffer, int size) {
        if (size > can_write_bytes_num()) {
            int new_size = { static_cast<int>(1.5 * (m_write_index + size)) };    
            resize_buffer(new_size);
        }
       std::memcpy(&m_buffer[m_write_index], buffer, size);
    }

    void TcpBuffer::resize_buffer(int new_size) {
        std::vector<char> new_buffer(new_size);
        int count = std::min(new_size, can_read_bytes_num());
        std::memcpy(&new_buffer[0], &m_buffer[m_read_index], count);
        m_buffer.swap(new_buffer);

        m_read_index = 0;
        m_write_index = m_read_index + count;
        adjust_buffer();
    }

    void TcpBuffer::read_from_buffer(std::vector<char>& re, int size) {
        if (can_read_bytes_num() == 0) { return; }
        
        int read_size  { std::min(size, can_read_bytes_num()) };
        
        std::vector<char> temp(read_size);
        std::memcpy(&temp[0], &m_buffer[m_read_index], read_size);
        re.swap(temp);
        m_read_index +=read_size;
    }

    void TcpBuffer::ajust_read_index(int size) {
        int j = m_read_index + size;
        if (j >= m_buffer.size()) {
            rpc::utils::ERROR_LOG(fmt::format("ajust_read_index() error, invalid size {}, old_read_index {}, buffer size{}",
            size, m_read_index, m_buffer.size()));
            return;
        }
        m_read_index = j;
        adjust_buffer();
        
    }

    void TcpBuffer::ajust_write_index(int size) {
       int j = m_write_index + size;
        if (j >= m_buffer.size()) {
            rpc::utils::ERROR_LOG(fmt::format("ajust_read_index() error, invalid size {}, old_read_index {}, buffer size{}",
            size, m_read_index, m_buffer.size()));
            return;
        }
        m_write_index = j;
        adjust_buffer();
    }

    void TcpBuffer::adjust_buffer() {
        if (m_read_index < static_cast<int>(m_buffer.size() / 3)) {
            return;
        }
        
        std::vector<char> new_buffer(m_buffer.size());

        int count { can_read_bytes_num() };
        std::memcpy(&new_buffer[0], &m_buffer[m_read_index], count);
        m_buffer.swap(new_buffer);
        m_read_index = 0;
        m_write_index = m_read_index + count;
    }
}