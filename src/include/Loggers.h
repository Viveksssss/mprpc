#pragma once
#include "LogQueue.h"
#include <chrono>
#include <string>
#include <thread>
#define FOREACH_LOG_LEVEL(f) f(TRACE) f(DEBUG) f(INFO) f(ERROR) f(FATAL)

/* 定义日志级别 */
#define _FUNCTION(name) name,
enum class LogLevel {
    FOREACH_LOG_LEVEL(_FUNCTION)
};
#undef _FUNCTION

struct LogMessage {
    LogLevel level;
    std::string msg;
    std::chrono::system_clock::time_point timestamp;
};

inline std::string log_level_name(LogLevel lev) {
    switch (lev) {
#define _FUNCTION(name)                                                                            \
    case LogLevel::name: return #name;
        FOREACH_LOG_LEVEL(_FUNCTION)
#undef _FUNCTION
    }
    return "unknown";
}

class Loggers {
public:
    static Loggers &GetInstance();
    ~Loggers();

    void SetLogLevel(LogLevel level);
    void log(LogLevel level, std::string msg);
    void set_path(std::string const &);
    std::string get_path();

    inline std::string get_level_name() {
        return log_level_name(_log_level);
    }

    void shutdown(); // 添加优雅关闭方法

private:
    static void signalHandler(int sig); // 信号处理器
    Loggers();
    Loggers(Loggers const &) = delete;
    Loggers &operator=(Loggers const &) = delete;

private:
    std::thread _worker;
    std::atomic<bool> _running;
    LogLevel _log_level;
    LogQueue<LogMessage> _queue;
    std::string _path;
    std::atomic<bool> _shutdown_requested{false}; // 关闭请求标志
};

#define _FUNCTION(level)                                                                           \
    template <typename... Args>                                                                    \
    inline void LOG_##level(std::format_string<Args...> fmt, Args &&...args) {                     \
        Loggers &log = Loggers::GetInstance();                                                     \
        std::string msg = std::format(fmt, std::forward<Args>(args)...);                           \
        log.log(LogLevel::level, msg);                                                             \
    }
FOREACH_LOG_LEVEL(_FUNCTION)
#undef _FUNCTION
