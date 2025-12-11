#include <iostream>
#include <thread>
#include <string>
#include <time.h>

using namespace std;

int factorial(int n) {
    int result = 1;
    for (int i = 1; i <= n; i++) {
        result *= i;
    }
    return result;
}

void Func(string name) {
    const int iterations = 10000000; 
    const int number = 10;          

    long long total = 0;
    for (int i = 0; i < iterations; i++) {
        total += factorial(number);
    }
}

int main() {
    clock_t start_parallel = clock();

    std::thread thread1(Func, "t1");
    std::thread thread2(Func, "t2");

    thread1.join();
    thread2.join();

    clock_t end_parallel = clock();
    double parallel_time = (double)(end_parallel - start_parallel) / CLOCKS_PER_SEC;
    cout << "Parallel time: " << parallel_time << " seconds" << endl << endl;
    //
    //
    //
    clock_t start_sequential = clock();

    Func("sequential1");
    Func("sequential2");

    clock_t end_sequential = clock();
    double sequential_time = (double)(end_sequential - start_sequential) / CLOCKS_PER_SEC;
    cout << "Sequential time: " << sequential_time << " seconds" << endl << endl;

    system("pause");
    return 0;
}