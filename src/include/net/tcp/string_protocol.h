#ifndef RPC_NET_TCP_STRING_PROTOCOL_H
#define RPC_NET_TCP_STRING_PROTOCOL_H

#include <string>
#include "net/tcp/abstract_protocol.h"

namespace rpc {
class StringProtocol : public AbstractProtocol {
public:
    std::string m_info;
};

} // namespace rpc
#endif