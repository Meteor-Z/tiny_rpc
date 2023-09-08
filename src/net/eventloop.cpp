#include "eventloop.hpp"
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <system_error>
#include <tinyxml/tinyxml.h>
#include "../common/log.hpp"
#include "../common/utils.hpp"
#include "fd_event.hpp"
#include "wakeup_fd_event.hpp"


namespace rpc
{
    static thread_local EventLoop* thread_current_eventloop;
    static constexpr int global_max_timeout = 10000;
    static constexpr int global_epoll_max = 10;
    EventLoop::EventLoop()
    {
        if (thread_current_eventloop != nullptr)
        {
            rpc::ERROR_LOG("创建eventloop失败,这个线程已经有了");
            exit(0);
        }

        m_thread_id = rpc::utils::get_thread_id(); // 得到当前的 id

        m_epoll_fd = epoll_create(10); // 随便传入 现代操作系统已经不用管这个数字了

        if (m_epoll_fd == -1) // 例程申请不了
        {
            ERROR_LOG(fmt::format("failed to epoll_create(),error info {}", errno));
            exit(0);
        }
      
        m_wakeup_fd = eventfd(0,EFD_NONBLOCK);
        if (m_wakeup_fd < 0)
        {
            ERROR_LOG(fmt::format("不能够创建eventfd ,error info {}", errno));
            exit(0);
        }

        init_wakeup_fd_event();

        INFO_LOG(fmt::format("在{}线程下成功创建", m_thread_id));
        thread_current_eventloop = this;
    }

    EventLoop::~EventLoop()
    {
        close(m_epoll_fd);
        if (m_wakeup_fd_event)
        {
            delete  m_wakeup_fd_event;
            m_wakeup_fd_event = nullptr;
        } 
    }
    void EventLoop::init_wakeup_fd_event()
    {
        m_wakeup_fd = eventfd(0, EFD_NONBLOCK);
        if (m_wakeup_fd < 0)
        {
            rpc::ERROR_LOG(fmt::format("failed to create eventloop,, eventfd create error ,error info {}", errno));
            exit(1);
        }

        rpc::INFO_LOG(fmt::format("wakeup fd = {}", m_wakeup_fd));
        
        m_wakeup_fd_event = new WakeUPEvent(m_wakeup_fd); // 事件唤醒
        m_wakeup_fd_event->listen(Fd_Event::TriggerEvent::IN_EVENT,[this]()
        {
            char buf[8];
            while (read(m_wakeup_fd, buf, 8) != -1 && errno != EAGAIN) { }
            rpc::DEBUG_LOG(fmt::format("read full bytes from wakeup fd {}", m_wakeup_fd));
        });

        add_epoll_event(m_wakeup_fd_event);
        
    }

    void EventLoop::loop()
    {
        while (!m_stop_flag)
        {
            // 缩到下面的unlock()那里就可以了
            std::unique_lock<std::mutex> lock { m_mtx };
            std::queue<std::function<void()>> temp_tasks;
            m_pending_tasks.swap(temp_tasks);
            lock.unlock();

            // 可能会有空值
            
            while (!temp_tasks.empty())
            {
                std::function<void()> cb = temp_tasks.front();
                temp_tasks.pop();
                if (cb) cb();
            }


            int timeout = global_max_timeout;
            
            epoll_event result_event [global_epoll_max];
           
            int rt = epoll_wait(m_epoll_fd, result_event, global_epoll_max, timeout); 
            rpc::DEBUG_LOG(fmt::format("now end epoll_wait.. rt = {}", rt));
            
            if (rt < 0) ERROR_LOG(fmt::format("epoll_wait error, errno = {}", errno));
            else 
            {
                for (int i = 0; i < rt; i++)
                {
                    epoll_event trigger_event = result_event[i];
                    std::unique_ptr<Fd_Event> fd_event_ptr = std::make_unique<Fd_Event>(*static_cast<Fd_Event*>(trigger_event.data.ptr));
                    if (fd_event_ptr == nullptr) 
                    {
                        rpc::ERROR_LOG("fd_event = nullptr, continue");
                        continue;
                    }

                    if (trigger_event.events & EPOLLIN) 
                    {

                        rpc::DEBUG_LOG(fmt::format("fd {} trigger EPOLLERROR event", fd_event_ptr->get_fd()));
                        add_task(fd_event_ptr->handler(Fd_Event::TriggerEvent::IN_EVENT));
                    }

                    if (trigger_event.events & EPOLLOUT)
                    {
                        rpc::DEBUG_LOG(fmt::format("fd {} trigger EPOLLIN event", fd_event_ptr->get_fd()));
                        add_task(fd_event_ptr->handler(Fd_Event::TriggerEvent::OUT_EVENT));
                    }
                }
            }
        }
    
    }

    void EventLoop::wake_up()
    {
        rpc::INFO_LOG("wakeup....");
        m_wakeup_fd_event->wakeup();
    }

    void EventLoop::stop()
    {
        m_stop_flag = true;
        wake_up();
    }
    void EventLoop::deal_wake_up() { }

    void EventLoop::add_epoll_event(Fd_Event* event)
    {
        if (is_in_loop_thread()) 
        {
            add_to_epoll(event);
        }else
        {
            auto cb = [this, event]() 
            {
                add_to_epoll(event);

            };
            add_task(cb, true);
        }
    }
    // yes
    void EventLoop::delete_epoll_event(Fd_Event* event)
    {
        if (is_in_loop_thread()) delete_from_epoll(event);
        else 
        {
            auto cb = [this, event]()
            {
                delete_from_epoll(event);
            };
            add_task(cb, true);
        }
    }
    bool EventLoop::is_in_loop_thread() { return m_thread_id == rpc::utils::get_thread_id(); }

    void EventLoop::add_task(std::function<void()> task, bool is_wake_up)
    {
        std::lock_guard<std::mutex> lock { m_mtx };
        m_pending_tasks.push(task);
        if (is_wake_up) wake_up();
        
    }
    // ok 
    void EventLoop::add_to_epoll(Fd_Event* event)
    {
        auto it = m_listen_fds.find(event->get_fd()); 
        int op = EPOLL_CTL_ADD; 

        if (it != m_listen_fds.end()) 
        { 
            op = EPOLL_CTL_MOD; 
        } 
        epoll_event tmp = event->get_epoll_event(); 

        rpc::INFO_LOG(fmt::format("epoll_event.events = {}", (int)tmp.events)); 
        int rt = epoll_ctl(m_epoll_fd, op, event->get_fd(), &tmp); // 注册 添加事件
        if (rt == -1)
        { 
            rpc::ERROR_LOG(fmt::format("failed epoll_ctl when add fd, errno={}, error={}", errno, strerror(errno))); 
        } 

        m_listen_fds.insert(event->get_fd()); 
        rpc::DEBUG_LOG(fmt::format("add event success, fd[{}]", event->get_fd())); 
    }

    void EventLoop::delete_from_epoll(Fd_Event* event)
    {
        auto it = m_listen_fds.find(event->get_fd()); 
        if (it == m_listen_fds.end())
        { 
            return; 
        } 
        int op = EPOLL_CTL_DEL; 
        epoll_event tmp = event->get_epoll_event(); 
        int rt = epoll_ctl(m_epoll_fd, op, event->get_fd(), nullptr); 
        if (rt == -1) 
        { 
            rpc::ERROR_LOG(fmt::format("failed epoll_ctl when add fd, errno={}, error={}", errno, strerror(errno))); 
        } 
        m_listen_fds.erase(event->get_fd()); 
        rpc::DEBUG_LOG(fmt::format("delete event success, fd[{}]", event->get_fd())); 

    }
}