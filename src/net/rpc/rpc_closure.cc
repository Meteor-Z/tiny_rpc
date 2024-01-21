#include "net/rpc/rpc_closure.h"

namespace rpc {
void RpcClosure::Run() {
    if (m_function) {
        m_function(); 
    }
}
} // namespace rpc