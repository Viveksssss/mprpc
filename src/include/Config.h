#pragma once

#include "muduo/noncopyable.h"
#include <string>
#include <unordered_map>

/**
 * @brief 配置文件类

    RpcServerIp RpcServerPort ZookeeperIp ZookeeperPort
 *
 */
class Config : noncopyable {
public:
    static Config &GetInstance();
    void LoadConfigFile(char const *file);
    std::string Get(std::string const &key);
    std::unordered_map<std::string, std::string> &GetMaps();

private:
    Config() { };
    std::unordered_map<std::string, std::string> _config;
};
