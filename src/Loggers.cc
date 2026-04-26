#include "include/Loggers.h"
#include <chrono>
#include <csignal>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <Logger.h>
#include <sstream>
#include <string>
#include <thread>

static std::atomic<Loggers *> g_logger_instance{nullptr};

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
    g_logger_instance.store(&instance); // 保存指针供信号处理
    return instance;
}

void Loggers::signalHandler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        std::cout << "\nReceived signal " << sig << ", shutting down gracefully..." << std::endl;
        if (auto *instance = g_logger_instance.load()) {
            instance->shutdown(); // 请求关闭
        }
        // 设置一个短暂的超时等待日志写入
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        exit(sig); // 退出
    }
}

void Loggers::shutdown() {
    _shutdown_requested = true;
    _running = false;
    _queue.break_wait();

    // 等待后台线程处理完所有消息（最多等待3秒）
    if (_worker.joinable()) {
        auto start = std::chrono::steady_clock::now();
        while (
            !_queue.empty() && std::chrono::steady_clock::now() - start < std::chrono::seconds(3)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        _worker.join();
    }
}

/**
 * @brief 指定log目录,默认在/tmp/mprpc下
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
void Loggers::log(LogLevel level, std::string msg) {
    _queue.push({level, msg, std::chrono::system_clock::now()});
}

Loggers::Loggers() : _running(true), _path("/tmp/mprpc") {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

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

            if (today != last_date) {
                std::string new_file = getLogFilePath(_path);

                std::error_code ec;
                std::filesystem::create_directories(
                    std::filesystem::path(new_file).parent_path(), ec);
                if (ec) {
                    std::cerr << "Can't create directory: " << ec.message() << std::endl;
                    continue;
                }

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

            LogMessage logMsg;
            if (_queue.pop_with_timeout(logMsg, std::chrono::milliseconds(100))) {
                auto msg_time = std::chrono::system_clock::to_time_t(logMsg.timestamp);
                std::tm msg_tm = *std::localtime(&msg_time);
                std::ostringstream time_oss;
                time_oss << std::put_time(&msg_tm, "%H:%M:%S");
                std::string str
                    = time_oss.str() + " [" + log_level_name(logMsg.level) + "] " + logMsg.msg;
                if (ofs.is_open()) {
                    ofs << str << "\n";
                } else {
                    std::cerr << str << std::endl;
                }
            }
        }

        LogMessage log_msg;
        while (_queue.try_pop(log_msg)) {
            if (!ofs.is_open() && current_file.empty()) {
                std::cerr << log_msg.msg << std::endl;
            } else {
                auto msg_time = std::chrono::system_clock::to_time_t(log_msg.timestamp);
                std::tm msg_tm = *std::localtime(&msg_time);
                std::ostringstream time_oss;
                time_oss << std::put_time(&msg_tm, "%H:%M:%S");
                std::string str
                    = time_oss.str() + " [" + log_level_name(log_msg.level) + "] " + log_msg.msg;

                ofs << str << "\n";
            }
        }
        if (ofs.is_open()) {
            ofs << std::flush;
        }
    });
}

Loggers::~Loggers() {
    shutdown();
}
