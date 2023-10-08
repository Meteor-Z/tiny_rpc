#include "net/io_thread/io_thread.hpp"
#include "common/log.hpp"
#include "common/utils.hpp"
#include "net/eventloop.hpp"
#include <fmt/core.h>
#include <pthread.h>
#include <cassert>
#include <semaphore.h>

namespace rpc 
{
    std::shared_ptr<EventLoop> IOThread::get_eventloop() { return m_event_loop; }

    void IOThread::start() { sem_post(&m_start_semphore); }
    
    void IOThread::join() { pthread_join(m_thread, nullptr); }
    
    IOThread::IOThread() 
    {
        // 初始化信号量
        int rt = sem_init(&m_init_semphore, 0, 0); 
        assert(rt == 0);

        rt = sem_init(&m_start_semphore, 0, 0);
        assert(rt == 0);

        // 创建一个线程，之后创建
        pthread_create(&m_thread, nullptr, &IOThread::Main, this);
    
        // 等到main函数已经执行完毕之后，然后才进行后面。
        sem_wait(&m_init_semphore); // 等到信号量 +1 才会进行返回
        
        rpc::utils::DEBUG_LOG(fmt::format("IOThread {} create success", m_thread_id));

        assert(rt == 0);
    }

    IOThread::~IOThread() 
    {
        m_event_loop->stop();
        sem_destroy(&m_init_semphore); // 信号量摧毁掉
        sem_destroy(&m_start_semphore);
        pthread_join(m_thread, nullptr); // 等待线程结束
    }

    void* IOThread::Main(void* args) 
    {
        std::shared_ptr<IOThread> thread_ptr { static_cast<IOThread*>(args) };
        thread_ptr->m_event_loop = std::make_shared<EventLoop>();
        thread_ptr->m_thread_id = rpc::utils::get_thread_id();

        // 唤醒等到的线程 这里的信号量指的是要初始化好这个线程
        sem_post(&thread_ptr->m_init_semphore);
        rpc::utils::DEBUG_LOG(fmt::format("IOThread {} wait start semaphore", thread_ptr->m_thread_id));
        
        // 这里是进行阻塞，直到 运行了 start 任务，才会开始 event loop 循环
        sem_wait(&thread_ptr->m_start_semphore); 
        rpc::utils::DEBUG_LOG(fmt::format("IOThread {} start loop", thread_ptr->m_thread_id));
        thread_ptr->m_event_loop->loop();
        
        return nullptr;
    }
}