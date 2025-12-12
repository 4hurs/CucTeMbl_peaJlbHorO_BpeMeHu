#include <coroutine>
#include <iostream>
#include <chrono>
#include <thread>


struct MyPromise
{
    int current_value = 0;

    auto get_return_object()
    {
        return std::coroutine_handle<MyPromise>::from_promise(*this);
    }

    std::suspend_always initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() {}

    std::suspend_always yield_value(int value)
    {
        current_value = value;
        return {};
    }
};

struct task
{
    std::coroutine_handle<MyPromise> handle;

    task(std::coroutine_handle<MyPromise> h) : handle(h) {}
    ~task() { if (handle) handle.destroy(); }

    void resume() { handle.resume(); }
    bool done() const { return handle.done(); }
    int get_value() const { return handle.promise().current_value; }
};

namespace std
{
    template<>
    struct coroutine_traits<task, int>
    {
        using promise_type = ::MyPromise;
    };
}

task long_computation(int steps)
{
    for (int i = 1; i <= steps; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        co_yield i;
    }
    co_return;
}

void print_progress(int current, int total, const std::string& name = "ADELINA")
{
    float percent = static_cast<float>(current) / total * 100.f;

    int name_length = name.length();
    int letters_to_show = static_cast<int>((percent / 100.f) * name_length);
    letters_to_show = std::max(0, std::min(name_length, letters_to_show));

    std::string displayed_name = name.substr(0, letters_to_show);

    std::cout << "\r[" << displayed_name << "=>] " << std::fixed << std::setprecision(1) << percent << "%";
    std::cout.flush();
}

int main()
{
    constexpr int TOTAL_STEPS = 100;
    auto coro = long_computation(TOTAL_STEPS);

    while (!coro.done())
    {
        coro.resume();
        int progress = coro.get_value();
        print_progress(progress, TOTAL_STEPS, "ADELINA");
        std::this_thread::yield();
    }
    std::cout << "\nDONE" << std::endl;
    return 0;
}