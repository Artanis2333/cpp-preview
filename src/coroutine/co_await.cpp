#include <coroutine>
#include <chrono>
#include <thread>
#include <functional>
#include <source_location>
#include <iostream>

#define PRINT_FUNC_NAME { auto loc = std::source_location::current(); std::cout << "[" << loc.function_name() << "]: " << std::endl; }
#define PRINT_FUNC_NAME_WITH(x) { auto loc = std::source_location::current(); std::cout << "[" << loc.function_name() << "]: " << x << std::endl; }

struct task_promise;

struct task
{
    using promise_type = task_promise;

    task() { PRINT_FUNC_NAME_WITH("this :" << this); }
    task(std::coroutine_handle<promise_type> h) : handle_(h) { PRINT_FUNC_NAME_WITH("this :" << this); std::cout << "handle: " << handle_.address() << std::endl; }
    ~task() { PRINT_FUNC_NAME_WITH("this :" << this); if (handle_ != nullptr) handle_.destroy(); }

    std::coroutine_handle<promise_type> handle_;
};

struct task_promise
{
    task_promise() { PRINT_FUNC_NAME_WITH("this :" << this); }
    task_promise(int value) : value_(value) { PRINT_FUNC_NAME_WITH("this :" << this << ", value: " << value); }
    ~task_promise() { PRINT_FUNC_NAME_WITH("this :" << this); }

    task get_return_object() { PRINT_FUNC_NAME; return task(); }
    auto initial_suspend() { PRINT_FUNC_NAME; return std::suspend_never(); }
    auto final_suspend() noexcept { PRINT_FUNC_NAME; return std::suspend_never(); }
    void return_void() { PRINT_FUNC_NAME; }
    void unhandled_exception() { PRINT_FUNC_NAME; std::terminate(); }

    static void* operator new(size_t size)
    {
        void* ptr = malloc(size);
        PRINT_FUNC_NAME_WITH("ptr: " << ptr << ", size: " << size << ", sizeof(task_promise): " << sizeof(task_promise));
        return ptr;
    }
    static void operator delete(void* ptr) noexcept
    {
        PRINT_FUNC_NAME_WITH("ptr: " << ptr);
        free(ptr);
    }

    int value_ = 0;
};

void async_operation(std::function<void(int)> callback)
{
    // 模拟异步操作
    std::thread([callback]() {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            callback(42); // 假设操作完成后返回42
            }).detach();
}

struct awaitable
{

    awaitable() { PRINT_FUNC_NAME_WITH("this :" << this); }
    ~awaitable() { PRINT_FUNC_NAME_WITH("this :" << this); }

    bool await_ready() const noexcept { PRINT_FUNC_NAME; return false; }
    void await_suspend(std::coroutine_handle<> handle)
    {
        PRINT_FUNC_NAME;
        async_operation([handle, this](int result) mutable {
                value_ = result;
                handle.resume();
                });
    }
    int await_resume() const noexcept { PRINT_FUNC_NAME; return value_; }

    int value_ = 0;
};

task async_operation_coroutine()
{
    int result = co_await awaitable();
    std::cout << "result: " << result << ", &result: " << &result << std::endl;
}

task async_operation_coroutine(int i)
{
    std::cout << "i: " << i << ", &i: " << &i << std::endl;
    int result = co_await awaitable();
    std::cout << "result: " << result << ", &result: " << &result << std::endl;
}

int main()
{
    //task t = async_operation_coroutine();
    task t = async_operation_coroutine(0);
    std::this_thread::sleep_for(std::chrono::seconds(2)); // 等待协程完成
    return 0;
}
