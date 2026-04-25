#pragma once

#include <functional>

class Defer {
public:
    template <typename F>
    explicit Defer(F &&func) : _function(std::forward<F>(func)) { }

    ~Defer() {
        std::invoke(_function);
    }

    Defer(Defer const &) = delete;
    Defer &operator=(Defer const &) = delete;
    Defer(Defer &&) noexcept = default;
    Defer &operator=(Defer &&) noexcept = default;

private:
    std::function<void()> _function;
};
