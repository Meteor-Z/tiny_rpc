#include "net/rpc/rpc_closure.h"

namespace rpc {
RpcClosure::RpcClosure(std::function<void()> function) : m_function(function) {}
void RpcClosure::Run() {
    if (m_function) {
        m_function();
    }
}
} // namespace rpc