// #include "time_event.hpp" 
#include "net/time/time_event.hpp"
#include "common/utils.hpp"
// #include "src/common/utils.hpp"
namespace rpc
{
   int64_t TimerEvent::get_arrive_time() { return m_arrive_time; }
   bool TimerEvent::is_cancel() { return m_is_cancel; }
   bool TimerEvent::is_repeat() { return m_is_repeat; } 
   void TimerEvent::set_cancel(bool value) { m_is_cancel = value; };
   std::function<void()> TimerEvent::get_callback() { return m_task; }

   void TimerEvent::reset_arrive_time()
   {
      m_arrive_time = rpc::utils::get_now_ms() + m_interval;
      rpc::utils::DEBUG_LOG(fmt::format("success create event, in execute at {}", m_arrive_time));
   }
    // 这是一个函数哦~
   TimerEvent::TimerEvent(int64_t interval, bool repeat, std::function<void()> task) 
   : m_interval(interval), m_is_repeat(repeat), m_task(task)
   {
      m_arrive_time = rpc::utils::get_now_ms() + m_interval; // 当前毫秒 + 间隔点
   }
   
}