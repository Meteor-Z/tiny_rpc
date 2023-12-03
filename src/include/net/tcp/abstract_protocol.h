/*
协议
rpc传输协议

*/
#ifndef RPC_NET_TCP_ABSTRACT_PROTOCOL_H
#define RPC_NET_TCP_ABSTRACT_PROTOCOL_H

#include <memory>
#include <string>
#include <string_view>
#include "common/log.h"

namespace rpc {
class AbstractProtocol : public std::enable_shared_from_this<AbstractProtocol> {
public:
    std::string get_req_id() const noexcept { return m_req_id; };
    void set_req_id(const std::string& req_id) noexcept { m_req_id = req_id; };
    virtual ~AbstractProtocol() { DEBUG_LOG("~AbstractProtocol"); }

protected:
    std::string m_req_id; // 标识的请求号
};

} // namespace rpc
#endif