#pragma once
#include "LogQueue.h"
#include <string>
#include <thread>

#define FOREACH_LOG_LEVEL(f) f(TRACE) f(DEBUG) f(INFO) f(ERROR) f(FATAL)

/* 定义日志级别 */
#define _FUNCTION(name) name,
enum class LogLevel {
    FOREACH_LOG_LEVEL(_FUNCTION)
};
#undef _FUNCTION

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
    void log(std::string msg);
    void set_path(std::string const &);
    std::string get_path();

    inline std::string get_level_name() {
        return log_level_name(_log_level);
    }

private:
    Loggers();
    Loggers(Loggers const &) = delete;
    Loggers &operator=(Loggers const &) = delete;

private:
    std::thread _worker;
    std::atomic<bool> _running;
    LogLevel _log_level;
    LogQueue<std::string> _queue;
    std::string _path;
};

#define _FUNCTION(level)                                                                           \
    template <typename... Args>                                                                    \
    inline void LOG_##level(std::format_string<Args...> fmt, Args &&...args) {                     \
        Loggers &log = Loggers::GetInstance();                                                     \
        log.SetLogLevel(Loggers::level);                                                           \
        std::string msg = std::format(fmt, std::forward<Args>(args)...);                           \
        log.log(msg);                                                                              \
    }                                                                                              \
    FOREACH_LOG_LEVEL(_FUNCTION)
#undef _FUNCTION
