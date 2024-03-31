#ifndef RPC_COMMON_SPIN_LOCK_H
#define RPC_COMMON_SPIN_LOCK_H

#include <atomic>

class SpinLock {
public:
    void lock() { while (flag.exchange(true, std::memory_order_acquire)); }
    void unlock() { flag.store(false, std::memory_order_release); }

private:
    std::atomic<bool> flag { false };
};
#endif