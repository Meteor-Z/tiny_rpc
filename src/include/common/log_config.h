/*
LogConfig.h
LogConfig初始化，给定一个地址，然后读取xml文件，

*/
#ifndef RPC_COMMON_CONFIG_H
#define RPC_COMMON_CONFIG_H

#include <memory>
#include <string>

namespace rpc {
/**
 * @brief 配置文件
 *
 */
class LogConfig {
public:
    // xmlfile:是一个xml文件路径，将其里面的等级进行初始化

    /**
     * @brief Construct a new Log Config object
     *
     * @param xmlfile 位置文件的目录
     */
    LogConfig(const std::string& xml_file);
    std::string& get_m_log_level();

public:
    static std::shared_ptr<LogConfig> GET_GLOBAL_CONFIG();
    // Log基本上是一个全局变量，然后共用这个Log
    static void SET_GLOBAL_CONFIG(const std::string& xml_file);

public:
    std::string m_log_level;    ///< 读取到的log_level
    std::string m_file_path {}; ///< 读取到的文件地址
    std::string m_file_name;    ///< 读取的文件名字
    int m_file_max_size {};     ///< 文件大小
    int m_log_sync_inteval {};  ///< 同步间隔 单位是 ms

    std::string m_ip {}; ///< 连接对方的ip地址
    int m_port {}; ///< 连接对方的端口
};
} // namespace rpc

#endif
