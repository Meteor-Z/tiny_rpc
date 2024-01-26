#include "net/rpc/rpc_controller.h"

namespace rpc {
void RpcController::Reset() {
    m_error_code = 0;
    m_error_info.clear();
    m_msg_id.clear();
    m_is_failed = false;
    m_is_cancled = false;

    m_local_addr = nullptr;
    m_peer_addr = nullptr;

    m_timeout_ms = 1000;
}


bool RpcController::Failed() const { return m_is_failed; }

std::string RpcController::ErrorText() const { return m_error_info; }

void RpcController::StartCancel() { m_is_cancled = true; }

void RpcController::SetFailed(const std::string& reason) { m_error_info = reason; }

bool RpcController::IsCanceled() const { return m_is_cancled; }

void RpcController::NotifyOnCancel(google::protobuf::Closure* callback) {}

void RpcController::set_error(int32_t error_code, const std::string& error_info) {
    m_error_code = error_code;
    m_error_info = error_info;
    // 设置调用失败了
    m_is_failed = true;
}

int32_t RpcController::get_error_code() { return m_error_code; }

std::string RpcController::get_error_info() { return m_error_info; }

void RpcController::set_msg_id(const std::string& msg_id) { m_msg_id = msg_id; }

std::string RpcController::get_msg_id() { return m_msg_id; }

void RpcController::set_local_addr(std::shared_ptr<IPv4NetAddr> local_addr) {
    m_local_addr = local_addr;
}

std::shared_ptr<IPv4NetAddr> RpcController::get_local_addr() { return m_local_addr; }

void RpcController::set_peer_addr(std::shared_ptr<IPv4NetAddr> peer_addr) {
    m_peer_addr = peer_addr;
}

std::shared_ptr<IPv4NetAddr> RpcController::get_peer_addr() { return m_peer_addr; }

void RpcController::set_timeout(int32_t timeout) { m_timeout_ms = timeout; }

int32_t RpcController::get_timeout() { return m_timeout_ms; }

} // namespace rpc
