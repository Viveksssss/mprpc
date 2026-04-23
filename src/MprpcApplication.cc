#include "include/MprpcApplication.h"
#include "include/Config.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>

namespace {

void ShowArgsHelp() {
    std::cout << "format: command -i <configfile>\n";
}

} // namespace

void MprpcApplication::Init(int argc, char **argv) {
    if (argc < 2) {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }
    std::string config_file;
    int c = 0;
    while ((c = getopt(argc, argv, "i:")) != -1) {
        switch (c) {
        case 'i': {
            config_file = optarg;
            break;
        }
        case '?': ShowArgsHelp(); exit(EXIT_FAILURE);
        default:  ShowArgsHelp(); exit(EXIT_FAILURE);
        }
    }
    _config.LoadConfigFile(config_file.c_str());
    // for (auto [k, v]: _config.GetMaps()) {
    //     std::cout << k << "=" << v << "\n";
    // }
}

Config &MprpcApplication::_config = Config::GetInstance();

MprpcApplication &MprpcApplication::GetInstance() {
    static MprpcApplication instance;
    return instance;
}

MprpcApplication::MprpcApplication() { }
