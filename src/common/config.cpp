#include "config.hpp"
#include <asm-generic/errno.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <tinyxml/tinyxml.h>
#include <memory>
#include <iostream>
#include <fmt/format.h>
namespace rpc
{
    static std::shared_ptr<Config> g_config { nullptr };

    static TiXmlElement* get_son_node(TiXmlDocument* father, const std::string& name)
    {
        if (!father->FirstChildElement(name.c_str()))
        {
            std::cout << fmt::format("获取 {} 出错", name);
            exit(1);
        }
        return father->FirstChildElement(name.c_str());
    }
    
    static TiXmlElement* get_son_node(TiXmlElement* father, const std::string& name)
    {
        if (!father->FirstChildElement(name.c_str()))
        {
            std::cout << "启动错误!" << std::endl;
            exit(1);
        }
        return father->FirstChildElement(name.c_str());
    }

    // tinyxml 只要delete 父亲节点就可以防止内存泄露了，所以这里没有使用智能指针，而是直接使用了裸指针。
    Config::Config(const std::string& xml_file)
    {
        std::unique_ptr<TiXmlDocument> xml_document_ptr = std::make_unique<TiXmlDocument>();
        bool rt = xml_document_ptr->LoadFile(xml_file.c_str());
        if (!xml_document_ptr->LoadFile(xml_file.c_str())) 
        {
            std::cout << "配置目录启动失败！不能正确读取到文件" << std::endl;
            exit(1);
        }
        // 这个是root节点的指针
        TiXmlElement* root_ptr = get_son_node(xml_document_ptr.get(), "root");
        TiXmlElement* root_log_ptr = get_son_node(root_ptr, "log");
        TiXmlElement* root_log_level_ptr = get_son_node(root_log_ptr, "log_level");
        m_log_level = root_log_level_ptr->GetText();

    }

    std::shared_ptr<Config> Config::get_global_config() { return g_config; }

    void  Config::set_global_config(const std::string& xml_file) { if (!g_config) g_config = std::make_shared<Config>(xml_file); }
    std::string& Config::get_m_log_level() { return m_log_level; }

}