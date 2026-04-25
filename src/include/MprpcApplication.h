#pragma once

/**
 * @brief 框架基础类
 *
 */

#include "MprpcApplication.h"
#include "MprpcChannel.h"
#include "MprpcController.h"
#include "MprpcProvider.h"
#include "muduo/noncopyable.h"

class Config;

class MprpcApplication : noncopyable {
public:
    static void Init(int argc, char **argv);
    static MprpcApplication &GetInstance();

private:
    MprpcApplication();
    static Config &_config;
};
