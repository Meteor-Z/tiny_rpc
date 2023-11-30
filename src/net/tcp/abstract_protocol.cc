#include <string_view>
#include "net/tcp/abstract_protocol.h"

namespace rpc {
std::string AbstractProtocol::get_req_id() const noexcept { return m_req_id; }
void AbstractProtocol::set_req_id(std::string_view req_id) noexcept {
    m_req_id = req_id;
};
} // namespace rpc