#include "include/Loggers.h"
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <Logger.h>
#include <sstream>
#include <string>
#include <thread>

namespace {

std::string getLogFilePath(std::string const &path) {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time_t_now);

    std::ostringstream oss;
    oss << std::put_time(&tm, "/%Y-%m/%Y-%m-%d"); // 按月和天分目录
    return path + oss.str() + ".log";
}

} // namespace

Loggers &Loggers::GetInstance() {
    static Loggers instance;
    return instance;
}

/**
 * @brief 指定log目录
 *
 * @param path
 */
void Loggers::set_path(std::string const &path) {
    _path = path;
}

std::string Loggers::get_path() {
    return _path;
}

void Loggers::SetLogLevel(LogLevel level) {
    _log_level = level;
}

/* 将日志信息写入LogQueue */
void Loggers::log(std::string msg) {
    _queue.push(std::move(msg));
}

Loggers::Loggers() {
    _path = ".";

    _worker = std::thread([this]() {
        std::string current_file;
        std::ofstream ofs;
        std::string last_date;

        while (_running) {
            auto now = std::chrono::system_clock::now();
            auto time_t_now = std::chrono::system_clock::to_time_t(now);
            std::tm tm = *std::localtime(&time_t_now);

            std::ostringstream oss;
            oss << std::put_time(&tm, "%Y-%m-%d");
            std::string today = oss.str();

            std::ostringstream ooss;
            ooss << std::put_time(&tm, "%H:%M:%S");
            std::string time = ooss.str();

            if (today != last_date) {
                std::string new_file = getLogFilePath(_path);

                std::filesystem::create_directories(std::filesystem::path(new_file).parent_path());

                if (ofs.is_open()) {
                    ofs.close();
                }
                ofs.open(new_file, std::ios::app);
                if (!ofs) {
                    std::cerr << "无法打开日志文件：" << new_file << std::endl;
                    continue;
                }

                current_file = new_file;
                last_date = today;
            }

            std::string msg;
            if (_queue.pop_with_timeout(msg, std::chrono::milliseconds(100))) {
                std::string str = time + " [" + log_level_name(_log_level) + "] " + msg;
                if (ofs.is_open()) {
                    ofs << str << "\n";
                } else {
                    std::cerr << str << std::endl;
                }
            }
        }

        std::string msg;
        while (_queue.try_pop(msg)) {
            if (!ofs.is_open() && current_file.empty()) {
                std::cerr << msg << std::endl;
            } else {
                auto now = std::chrono::system_clock::now();
                auto time_t_now = std::chrono::system_clock::to_time_t(now);
                std::tm tm = *std::localtime(&time_t_now);

                std::ostringstream ooss;
                ooss << std::put_time(&tm, "%H:%M:%S");
                std::string time = ooss.str();

                std::string str = time + " [" + log_level_name(_log_level) + "] " + msg;
                ofs << str << "\n";
            }
        }
        if (ofs.is_open()) {
            ofs << std::flush;
        }
    });
}

Loggers::~Loggers() {
    _running = false;
    _queue.break_wait();
    if (_worker.joinable()) {
        _worker.join();
    }
}
