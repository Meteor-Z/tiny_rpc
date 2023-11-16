#include <pthread.h>
#include <cassert>
#include <semaphore.h>
#include <fmt/core.h>
#include "net/io_thread/io_thread.h"
#include "common/log.h"
#include "common/utils.h"
#include "net/eventloop.h"

namespace rpc {
std::shared_ptr<EventLoop> IOThread::get_eventloop() const noexcept {
    return m_event_loop;
}

// 将信号量post以下就可以直接启动了
void IOThread::start() { sem_post(&m_start_semphore); }

void IOThread::join() { pthread_join(m_thread, nullptr); }

IOThread::IOThread() {
    // 初始化
    int rt = sem_init(&m_init_semphore, 0, 0);
    assert(rt == 0);

    rt = sem_init(&m_start_semphore, 0, 0);
    assert(rt == 0);

    // 创建一个线程， 会开始执行
    pthread_create(&m_thread, nullptr, &IOThread::Main, this);

    // 等到main函数已经执行完毕之后，然后才进行后面。
    // 等待当前线程
    sem_wait(&m_init_semphore);

    DEBUG_LOG(fmt::format("IOThread {} create success", m_thread_id));

    assert(rt == 0);
}

IOThread::~IOThread() {
    m_event_loop->stop();
    sem_destroy(&m_init_semphore); // 信号量摧毁掉
    sem_destroy(&m_start_semphore);
    pthread_join(m_thread, nullptr); // 等待线程结束
}

void* IOThread::Main(void* args) {
    // --------- 封装 ---------------
    std::shared_ptr<IOThread> thread_ptr { static_cast<IOThread*>(args) };
    thread_ptr->m_event_loop = std::make_shared<EventLoop>();
    thread_ptr->m_thread_id = rpc::utils::get_thread_id();
    // ------------------------

    // 唤醒等到的线程 这里的信号量指的是要初始化好这个线程
    sem_post(&thread_ptr->m_init_semphore);
    DEBUG_LOG(
        fmt::format("IOThread {} wait start semaphore", thread_ptr->m_thread_id));

    // 进行堵塞，等到信号才能进行loop循环
    sem_wait(&thread_ptr->m_start_semphore);
    
    DEBUG_LOG(fmt::format("IOThread {} start loop", thread_ptr->m_thread_id));

    thread_ptr->m_event_loop->loop();

    return nullptr;
}
} // namespace rpc