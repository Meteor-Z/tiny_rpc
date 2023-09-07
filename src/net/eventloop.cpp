#include "eventloop.hpp"
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <functional>
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
    static thread_local EventLoop* thread_current_eventloop { nullptr };
    static constexpr int global_max_timeout = 10000;
    static constexpr int global_epoll_max = 10;
    EventLoop::EventLoop()
    {
        if (thread_current_eventloop != nullptr)
        {
            rpc::ERROR_LOG("创建eventloop失败,这个线程已经有了");
            exit(0);
        }
        m_thread_id = rpc::utils::get_thread_id();

        m_epoll_fd = epoll_create(114514); // 随便传入

        if (m_epoll_fd == -1)
        {
            ERROR_LOG(fmt::format("failed to epoll_create(),error info {}", errno));
            exit(0);
        }
      
        m_wake_up_fd = eventfd(0,EFD_NONBLOCK);
        if (m_wake_up_fd < 0)
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
            delete m_wakeup_fd_event;
            m_wakeup_fd_event = nullptr;
        } 
    }
    void EventLoop::init_wakeup_fd_event()
    {
        m_wakeup_fd_event = new WakeUPEvent(m_wake_up_fd);
        auto read_callback = [&]() 
        {
            char buf [8];
            while (read(m_wake_up_fd, buf, 8) != -1 && errno != EAGAIN) { }
            DEBUG_LOG(fmt::format("read full bytes from wakeup fd[{}]", m_wake_up_fd));
        };

        m_wakeup_fd_event->listen(Fd_Event::TriggerEvent::IN_EVENT, read_callback);

        add_epoll_event(m_wakeup_fd_event);
    }
    void EventLoop::loop()
    {
        while (!m_stop_flag)
        {
            std::lock_guard<std::mutex> lock { m_mtx };
            std::queue<std::function<void()>> temp_tasks = m_pending_tasks;
            m_pending_tasks.swap(temp_tasks); // ?
            while (!temp_tasks.empty())
            {
                temp_tasks.front()(); // 执行这个函数
                temp_tasks.pop();
            }
            int timeout = global_max_timeout;
            
            epoll_event result_event [global_epoll_max];
            int rt = epoll_wait(m_epoll_fd, result_event, global_epoll_max, timeout);
            if (rt < 0) ERROR_LOG(fmt::format("epoll_wait error, errno = {}", errno));
            else 
            {
                for (int i = 0; i < rt; i++)
                {
                    epoll_event trigger_event = result_event[i];
                    Fd_Event* fd_event = static_cast<Fd_Event*>(trigger_event.data.ptr);
                    if (fd_event == nullptr) continue;
                    if (trigger_event.events | EPOLLIN) 
                    {
                        add_task(fd_event->handler(Fd_Event::TriggerEvent::IN_EVENT));
                        
                    } else if (trigger_event.events | EPOLLOUT)
                    {
                        add_task(fd_event->handler(Fd_Event::TriggerEvent::OUT_EVENT));
                    }
                }
            }
        }
    }

    void EventLoop::wake_up()
    {
        m_wakeup_fd_event->wakeup();
    }

    void EventLoop::stop()
    {
        m_stop_flag = true;
    }
    void EventLoop::deal_wake_up()
    {

    }
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
    // 只是将所有的事件放在这个任务里面
    void EventLoop::delete_epoll_event(Fd_Event* event)
    {
        if (is_in_loop_thread()) delete_from_epoll(event);
        else 
        {
            auto cb = [this, event]()
            {
                delete_epoll_event(event);
            };
            add_task(cb, true);
        }
    }
    bool EventLoop::is_in_loop_thread()
    {
        return m_thread_id == rpc::utils::get_thread_id();
    }
    void EventLoop::add_task(std::function<void()> task, bool is_wake_up)
    {
        std::lock_guard<std::mutex> lock { m_mtx };
        m_pending_tasks.push(task);
        if (is_wake_up) wake_up();
        
    }
    void EventLoop::add_to_epoll(Fd_Event* event)
    {
        auto it = m_listen_fds.find(event->get_fd());
        int op = EPOLL_CTL_ADD;
        if (it != m_listen_fds.end())
        {
            op = EPOLL_CTL_MOD;
        }
        epoll_event temp = event->get_epoll_event();
        int rt = epoll_ctl(m_epoll_fd, op, event->get_fd(), &temp);
        if (rt == -1)
        {
            ERROR_LOG(fmt::format("failed epoll_ctl when add fd, error = {} error = ", errno, strerror(errno)));
        }
        INFO_LOG(fmt::format("add succes {}", event->get_fd()));
    }

    void EventLoop::delete_from_epoll(Fd_Event* event)
    {
        auto it = m_listen_fds.find(event->get_fd());
        if (it == m_listen_fds.end()) return;
        int op = EPOLL_CTL_DEL;
        epoll_event temp= event->get_epoll_event();
        int rt = epoll_ctl(m_epoll_fd, op, event->get_fd(), &temp);
        if (rt == -1)
        {
            ERROR_LOG(fmt::format("failed epoll_ctl when delete fd, errno = {} error = {}", errno, strerror(errno)));
        }
         INFO_LOG(fmt::format("delete succes {}", event->get_fd()));

    }
}