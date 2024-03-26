/**
 * @file time_event.h
 * @author liuzechen.coder (liuzechen.coder@qq.com)
 * @brief 定时器的主要任务
 * @version 0.1
 * @date 2024-03-14
 * @note 定时器的主要实现，当前时间大于arrive_time的时候，就会触发这个定时任务，
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#ifndef RPC_NET_TIME_EVENT_H
#define RPC_NET_TIME_EVENT_H

#include <cstdint>
#include <functional>

namespace rpc {
class TimerEvent {
public:
    /**
     * @brief Construct a new Timer Event object
     *
     * @param interval 时间间隔
     * @param repeat true表示重复， false表示不重复
     * @param task 任务
     */
    TimerEvent(int64_t interval, bool repeat, std::function<void()> task);

    TimerEvent() = delete;
    TimerEvent(const TimerEvent&) = delete;
    TimerEvent(TimerEvent&&) = delete;
    TimerEvent& operator=(const TimerEvent&) = delete;
    TimerEvent& operator=(TimerEvent&&) = delete;

    ~TimerEvent() = default;

    /**
     * @brief 得到要执行的时间
     *
     * @return int64_t 时间戳
     */
    int64_t get_arrive_time() const noexcept;

    /**
     * @brief 是否取消
     *
     * @return true 取消
     * @return false 不取消
     */
    bool is_cancel() const noexcept;

    /**
     * @brief 是否重复
     *
     * @return true 重复
     * @return false 不重复
     */
    bool is_repeat() const noexcept;

    /**
     * @brief 设置取消
     *
     * @param value 取消
     */
    void set_cancel(bool value);

    /**
     * @brief 得到要执行的回调函数
     *
     * @return std::function<void()> 回调函数
     */
    std::function<void()> get_callback();

    /**
     * @brief 回复
     *
     */
    void reset_arrive_time();

private:
    int64_t m_arrive_time; ///< 时间点，当到达这个时间戳的时候就会执行 单位是 ms
    int64_t m_interval; ///< 事件间隔，每隔多长时间执行一次这个事件 单位是 ms
    bool m_is_repeat { false };   ///< 任务是否重复
    bool m_is_cancel { false };   ///< 这个任务是否取消
    std::function<void()> m_task; ///< 要执行的回调函数任务
};
} // namespace rpc

#endif
