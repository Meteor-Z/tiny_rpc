#include "common/log.h"
#include "common/log_config.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <fmt/core.h>
#include <fmt/format.h>
#include <iostream>
#include <memory>
#include <tinyxml/tinyxml.h>

namespace rpc {
static std::shared_ptr<LogConfig> g_log_config{nullptr};

std::shared_ptr<LogConfig> LogConfig::GET_GLOBAL_CONFIG() { return g_log_config; }

void LogConfig::SET_GLOBAL_CONFIG(const std::string &xml_file) {
    if (!g_log_config) {
        g_log_config = std::make_shared<LogConfig>(xml_file);
    }
}

std::string &LogConfig::get_m_log_level() { return m_log_level; }

static TiXmlElement *get_son_node(TiXmlDocument *father,
                                  const std::string &name) {
    if (!father->FirstChildElement(name.c_str())) {
        std::cout << fmt::format("获取 {} 出错", name);
        exit(1);
    }
    return father->FirstChildElement(name.c_str());
}

static TiXmlElement *get_son_node(TiXmlElement *father,
                                  const std::string &name) {
    if (!father->FirstChildElement(name.c_str())) {
        fmt::println("初始化配置log错误,启动错误!");
        exit(1);
    }
    return father->FirstChildElement(name.c_str());
}

// tinyxml 只要delete
// 父亲节点就可以防止内存泄露了，所以这里没有使用智能指针，而是直接使用了裸指针。
LogConfig::LogConfig(const std::string &xml_file) {
    std::unique_ptr<TiXmlDocument> xml_document_ptr =
        std::make_unique<TiXmlDocument>();
    bool rt = xml_document_ptr->LoadFile(xml_file.c_str());

    if (!xml_document_ptr->LoadFile(xml_file.c_str())) {
        std::cout << "配置目录启动失败！不能正确读取到文件" << std::endl;
        exit(1);
    }

    // 这个是root节点的指针
    TiXmlElement *root_ptr = get_son_node(xml_document_ptr.get(), "root");
    TiXmlElement *root_log_ptr = get_son_node(root_ptr, "log");
    TiXmlElement *root_log_level_ptr = get_son_node(root_log_ptr, "log_level");
    m_log_level = root_log_level_ptr->GetText();
}
} // namespace rpc