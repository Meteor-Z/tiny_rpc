#ifndef RPC_NET_RPC_RPC_CLOSURE_H
#define RPC_NET_RPC_RPC_CLOSURE_H

#include <functional>
#include "google/protobuf/stubs/callback.h"

namespace rpc {
class RpcClosure : public google::protobuf::Closure {
public:
    void Run() override;

private:
    std::function<void()> m_function { nullptr };
};
} // namespace rpc
#endif
