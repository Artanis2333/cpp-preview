#include <iostream>
#include <coroutine>
#include <memory>

// 定义一个生成器类
template<typename T>
class generator {
public:
    // 协程的 promise 类型
    struct promise_type {
        T value; // 用于存储生成的值

        // 协程开始时调用
        generator get_return_object() {
            return generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        // 初始挂起点
        std::suspend_always initial_suspend() noexcept { return {}; }

        // 最终挂起点
        std::suspend_always final_suspend() noexcept { return {}; }

        // 处理 co_yield
        std::suspend_always yield_value(T val) noexcept {
            value = val; // 存储生成的值
            return {};
        }

        // 处理 co_return
        void return_void() noexcept {}

        // 处理未捕获的异常
        void unhandled_exception() { std::terminate(); }
    };

    // 协程句柄
    std::coroutine_handle<promise_type> coro;

    // 构造函数
    explicit generator(std::coroutine_handle<promise_type> h) : coro(h) {}

    // 析构函数
    ~generator() {
        if (coro) coro.destroy();
    }

    // 迭代器支持
    struct iterator {
        std::coroutine_handle<promise_type> coro;

        // 解引用操作符
        T operator*() const {
            return coro.promise().value;
        }

        // 前置递增操作符
        iterator& operator++() {
            coro.resume(); // 恢复协程
            if (coro.done()) coro = nullptr; // 如果协程结束，置空句柄
            return *this;
        }

        // 不等于操作符
        bool operator!=(const iterator& other) const {
            return coro != other.coro;
        }
    };

    // 返回迭代器
    iterator begin() {
        if (coro) {
            coro.resume(); // 恢复协程以生成第一个值
            if (coro.done()) coro = nullptr; // 如果协程结束，置空句柄
        }
        return iterator{coro};
    }

    // 结束迭代器
    iterator end() {
        return iterator{nullptr};
    }
};

// 定义一个生成器函数
generator<int> generate_numbers(int start, int end) {
    for (int i = start; i <= end; ++i) {
        co_yield i; // 生成一个值并暂停
    }
}

// 主函数
int main() {
    // 使用生成器
    for (int num : generate_numbers(1, 5)) {
        std::cout << num << " "; // 输出生成的数字
    }
    std::cout << std::endl;
    return 0;
}
