#include "include/Config.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

namespace {

std::string trim(std::string const &str) {
    auto start = str.find_first_not_of(" \t");
    if (start == std::string::npos) {
        return "";
    }
    auto end = str.find_last_not_of(" \t");
    return str.substr(start, end - start + 1);
}

std::pair<std::string, std::string> parse(std::string const &line) {
    auto idx = line.find('=');
    if (idx == -1) {
        return {};
    }
    std::string key;
    std::string value;
    key = trim(line.substr(0, idx));
    value = trim(line.substr(idx + 1));
    return {key, value};
}

} // namespace

Config &Config::GetInstance() {
    static Config instance;
    return instance;
}

void Config::LoadConfigFile(char const *file) {
    std::ifstream ifs(file);
    if (!ifs.is_open()) {
        std::cout << file << "open failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(ifs, line)) {
        line = trim(line);

        if (line.empty() || line[0] == '#') {
            continue;
        }
        auto kv = parse(line);
        _config.insert(kv);
    }
}

std::string Config::Get(std::string const &key) {
    if (auto it = _config.find(key); it != _config.end()) {
        return it->second;
    }
    return {};
}

std::unordered_map<std::string, std::string> &Config::GetMaps() {
    return _config;
}
