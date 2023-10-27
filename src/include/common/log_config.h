/*
LogConfig.h
LogConfig初始化，给定一个地址，然后读取xml文件，





*/

#ifndef RPC_COMMON_CONFIG_H
#define RPC_COMMON_CONFIG_H

#include <map>
#include <memory>
#include <string>

namespace rpc {
class LogConfig {
public:
    // xmlfile:是一个xml文件路径，将其里面的等级进行初始化
    LogConfig(const std::string &xmlfile);
    std::string &get_m_log_level();

public:
    static std::shared_ptr<LogConfig> GET_GLOBAL_CONFIG();
    // Log基本上是一个全局变量，然后共用这个Log
    static void SET_GLOBAL_CONFIG(const std::string &xml_file);

private:
    std::string m_log_level;
};
} // namespace rpc

#endif