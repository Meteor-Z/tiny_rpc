#ifndef RPC_COMMON_CONFIG_H
#define RPC_COMMON_CONFIG_H

#include <map>
#include <string>

namespace rpc
{
    class Config
    {
    public:
        Config(const std::string& xmlfile);
        static Config* get_global_config();
        static void set_global_config(const std::string& xml_file);
        std::string& get_m_log_level();
    private:
        std::string m_log_level;

    };
}

#endif