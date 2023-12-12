/*
rpc分发器。将接收到的包分开出去

*/
#ifndef RPC_NET_RPC_RPC_DISPATCHOR_H
#define RPC_NET_RPC_RPC_DISPATCHOR_H

#include <memory>
#include <string>
#include <map>
#include "net/coder/abstract_protocol.h"
#include "google/service.h"

namespace rpc {
class RpcDispatcher {
public:
    // 进行分发
    void dispatcher(std::shared_ptr<AbstractProtocol> request,
                    std::shared_ptr<AbstractProtocol> response);

private:
    std::map<std::string, std::shared_ptr<>>
};
} // namespace rpc

#endif