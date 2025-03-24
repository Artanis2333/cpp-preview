#include <coroutine>
#include <thread>
#include <chrono>
#include <functional>
#include <iostream>

struct Task {
    struct promise_type {
        Task get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };
};

void async_operation1(std::function<void(int)> callback) {
    std::thread([callback]() {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            callback(42); // 第一个操作返回42
            }).detach();
}

void async_operation2(std::function<void(int)> callback) {
    std::thread([callback]() {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            callback(100); // 第二个操作返回100
            }).detach();
}

struct Awaitable1 {
    int value;

    Awaitable1() : value(0) {}

    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> handle) {
        async_operation1([handle, this](int result) mutable {
                value = result;
                //handle.resume();
                });
    }

    int await_resume() const noexcept { return value; }
};

struct Awaitable2 {
    int value;

    Awaitable2() : value(0) {}

    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> handle) {
        async_operation2([handle, this](int result) mutable {
                value = result;
                handle.resume();
                });
    }

    int await_resume() const noexcept { return value; }
};

template <typename... Awaitables>
struct WhenAll {
    std::tuple<Awaitables...> awaitables;

    WhenAll(Awaitables&&... awaitables)
        : awaitables(std::forward<Awaitables>(awaitables)...) {}

    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> handle) {
        std::apply([handle](auto&... awaitable) {
                (awaitable.await_suspend(handle), ...);
                }, awaitables);
    }

    auto await_resume() {
        return std::apply([](auto&... awaitable) {
                return std::make_tuple(awaitable.await_resume()...);
                }, awaitables);
    }
};

template <typename... Awaitables>
WhenAll<Awaitables...> when_all(Awaitables&&... awaitables) {
    return WhenAll<Awaitables...>(std::forward<Awaitables>(awaitables)...);
}

Task async_operations_parallel_coroutine() {
    auto [result1, result2] = co_await when_all(Awaitable1{}, Awaitable2{});

    std::cout << "Result 1: " << result1 << std::endl;
    std::cout << "Result 2: " << result2 << std::endl;
}

int main() {
    async_operations_parallel_coroutine();
    std::this_thread::sleep_for(std::chrono::seconds(3)); // 等待协程完成
    return 0;
}
