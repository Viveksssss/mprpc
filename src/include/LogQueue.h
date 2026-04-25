#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>

/**
 * @brief 异步写日志队列
 *
 */

template <typename T>
class LogQueue {
public:
    void push(T data) {
        std::lock_guard<std::mutex> lock(_mtx);
        _queue.push(std::move(data));
    }

    T pop() {
        std::unique_lock<std::mutex> lock(_mtx);
        while (_queue.empty()) {
            _cv.wait(lock);
        }
        T data = std::move(_queue.front());
        _queue.pop();
        return data;
    }

    bool pop_with_timeout(T &value, std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(_mtx);

        if (!_cv.wait_for(lock, timeout, [this]() { return !_queue.empty() || _broken; })) {
            return false;
        }

        if (_broken && _queue.empty()) {
            return false;
        }
        value = std::move(_queue.front());
        _queue.pop();
        return true;
    }

    bool try_pop(T &value) {
        std::lock_guard<std::mutex> lock(_mtx);
        if (_queue.empty()) {
            return false;
        }
        value = std::move(_queue.front());
        _queue.pop();
        return true;
    }

    void break_wait() {
        _broken = true;
        _cv.notify_all();
    }

private:
    std::queue<T> _queue;
    std::mutex _mtx;
    std::condition_variable _cv;
    std::atomic<bool> _broken{false};
};
