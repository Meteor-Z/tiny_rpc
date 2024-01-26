#ifndef RPC_NET_RPC_RPC_CLOSURE_H
#define RPC_NET_RPC_RPC_CLOSURE_H

#include <functional>
#include "google/protobuf/stubs/callback.h"

namespace rpc {
class RpcClosure : public google::protobuf::Closure {
public:
    /**
     * @brief Construct a new Rpc Closure object
     * 
     * @param function 
     */
    RpcClosure(std::function<void()> function);
    
    /**
     * @brief 重载了 google::protobuf::Closure
     * 
     */
    void Run() override;

private:
    std::function<void()> m_function { nullptr };
};
} // namespace rpc
#endif
