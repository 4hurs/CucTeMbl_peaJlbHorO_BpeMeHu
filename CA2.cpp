#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <chrono>

//std::mutex m;

void Func(std::string name) {
    //long double для точности
    long double i = 0.0;
    const long double increment = 1e-9; 

    auto start_time = std::chrono::high_resolution_clock::now();

    //цикл на 1 сек
    while (std::chrono::high_resolution_clock::now() - start_time < std::chrono::seconds(1)) {
        i += increment;
    }

    //блок мьютекса m.lock();
    std::cout << "Thread " << name << " finished with i = " << i << std::endl;
    //разблок мьютекса m.unlock();
}

int main() {
    std::cout.precision(10);

    std::thread thread1(Func, "t1");
    std::thread thread2(Func, "t2");
    std::thread thread3(Func, "t3");

    thread1.join();
    thread2.join();
    thread3.join();

    return 0;
}