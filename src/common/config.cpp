#include "config.hpp"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <tinyxml/tinyxml.h>
#include <memory>
#include <iostream>
#include <fmt/format.h>
namespace rpc
{
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
        TiXmlDocument* xml_document_ptr = new TiXmlDocument();
        bool rt = xml_document_ptr->LoadFile(xml_file.c_str());
        if (!xml_document_ptr->LoadFile(xml_file.c_str())) 
        {
            std::cout << "配置目录启动失败！不能正确读取到文件" << std::endl;
            exit(1);
        }
        // 这个是root节点的指针
        TiXmlElement* root_ptr = get_son_node(xml_document_ptr, "root");
        TiXmlElement* root_log_ptr = get_son_node(root_ptr, "log");
        TiXmlElement* root_log_level_ptr = get_son_node(root_log_ptr, "log_level");
        std::cout << root_log_level_ptr->GetText() << std::endl;
        delete xml_document_ptr; // 防止内存泄露

    }
}