#include <coroutine>
#include <iostream>
#include <exception>

struct Task {
    struct promise_type {
        std::exception_ptr exception; // 用于存储异常

        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {
            std::cout << "unhandled_exception() called" << std::endl;
            exception = std::current_exception(); // 捕获并存储异常
        }
    };

    std::coroutine_handle<promise_type> handle;

    Task(std::coroutine_handle<promise_type> h) : handle(h) {}
    ~Task() {
            std::cout << "~Task" << std::endl;
        if (handle) handle.destroy();
    }

    void resume() {
        if (handle && !handle.done()) {
            std::cout << "resume" << std::endl;
            handle.resume();
        }
        if (handle.promise().exception) {
            std::rethrow_exception(handle.promise().exception); // 重新抛出异常
        }
    }
};

Task example_coroutine() {
    std::cout << "Coroutine started" << std::endl;
    throw std::runtime_error("Something went wrong!"); // 抛出异常
    co_return;
}

int main() {
    try {
        Task task = example_coroutine(); // 创建协程
        task.resume(); // 恢复协程
    } catch (const std::exception& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }
    return 0;
}
