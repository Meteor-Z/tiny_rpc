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
    private:
        std::map<std::string, std::string> m_config_values;

    };
}

#endif