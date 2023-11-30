/*
协议
rpc传输协议

*/
#ifndef RPC_NET_TCP_ABSTRACT_PROTOCOL_H
#define RPC_NET_TCP_ABSTRACT_PROTOCOL_H

#include <memory>
#include <string>
#include <string_view>
namespace rpc {
class AbstractProtocol {
public:
    std::string get_req_id() const  noexcept;
    void set_req_id(std::string_view req_id) noexcept;

protected:
    std::string m_req_id; //标识的请求号
};

} // namespace rpc
#endif