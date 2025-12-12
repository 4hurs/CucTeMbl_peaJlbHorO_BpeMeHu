#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>

std::mutex m;
std::atomic<bool> cook_finished(false);
std::atomic<bool> cook_fired(false);
std::atomic<bool> all_fatmen_burst(false);

// Тарелки
int dish1 = 3000;
int dish2 = 3000;
int dish3 = 3000;

// Съедено каждым толстяком
int fatman1_eaten = 0;
int fatman2_eaten = 0;
int fatman3_eaten = 0;

const int gluttony = 100;          
const int efficiency_factor = 1000; 

//флаги для проверок
std::atomic<bool> time_to_eat(true);
std::atomic<bool> time_to_cook(false);
std::atomic<int> fatmans_ate[3]{};

void fatman(int fatman_id, int& dish, int& eaten) {
    while (!cook_fired && !all_fatmen_burst && !cook_finished) {
        while ((!time_to_eat || fatmans_ate[fatman_id - 1]) && !cook_fired && !all_fatmen_burst && !cook_finished) {
            std::this_thread::yield();
        }
        if (cook_fired || all_fatmen_burst || cook_finished) {
            break;
        }

        m.lock();

        if (eaten >= 10000) {
            std::cout << "Толстяк " << fatman_id << " лопнул! Съел: " << eaten << std::endl;
            m.unlock();
            break;
        }

        if (dish >= gluttony) {
            dish -= gluttony;
            eaten += gluttony;
            fatmans_ate[fatman_id - 1] = true;
            std::cout << "Толстяк " << fatman_id << " съел " << gluttony
                << ". Осталось: " << dish << ", Всего съел: " << eaten << std::endl;
        }
        else if (dish < gluttony && !cook_fired) {
            std::cout << "!!! Толстяк " << fatman_id << " остался без еды! Кук уволен !!!" << std::endl;
            cook_fired = true;
            cook_finished = true;
            m.unlock();
            break;
        }

        if (fatmans_ate[0] and fatmans_ate[1] and fatmans_ate[2]) {
            time_to_cook = true;
            time_to_eat = false;
        }
        m.unlock();

        while ((time_to_cook or fatmans_ate[fatman_id - 1]) && !cook_fired && !all_fatmen_burst && !cook_finished) {
            std::this_thread::yield();
        }
    }
    m.lock();
    if (!cook_fired && !all_fatmen_burst) {
        std::cout << "Толстяк " << fatman_id << " закончил. Итого съел: " << eaten << std::endl;
    }
    m.unlock();
}

void cook() {
    auto start = std::chrono::steady_clock::now();

    while (!cook_fired && !all_fatmen_burst) {
        if (std::chrono::steady_clock::now() - start >= std::chrono::seconds(5)) {
            cook_finished = true;
            break;
        }
        while (!time_to_cook && !cook_fired && !all_fatmen_burst) {
            if (std::chrono::steady_clock::now() - start >= std::chrono::seconds(5)) {
                cook_finished = true;
                break;
            }
            std::this_thread::yield();
        }
        if (cook_fired || all_fatmen_burst || cook_finished) {
            break;
        }
        m.lock();

        dish1 += efficiency_factor;
        dish2 += efficiency_factor;
        dish3 += efficiency_factor;

        time_to_cook = false;
        time_to_eat = true;

        for (int i = 0; i < 3; ++i) {
            fatmans_ate[i] = false;
        }
        std::cout << "Кук добавил по " << efficiency_factor << " наггетсов. "
            << "Тарелки: " << dish1 << ", " << dish2 << ", " << dish3 << std::endl;

        m.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    if (!cook_fired && !all_fatmen_burst) {
        cook_finished = true;
        return;
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    std::cout << "Коэффициенты: gluttony=" << gluttony << ", efficiency=" << efficiency_factor << std::endl;
    std::cout << "Начальное количество: 3000 наггетсов в каждой тарелке" << std::endl;

    std::thread cook_thread(cook);
    std::thread fatman1_thread(fatman, 1, std::ref(dish1), std::ref(fatman1_eaten));
    std::thread fatman2_thread(fatman, 2, std::ref(dish2), std::ref(fatman2_eaten));
    std::thread fatman3_thread(fatman, 3, std::ref(dish3), std::ref(fatman3_eaten));

    fatman1_thread.join();
    fatman2_thread.join();
    fatman3_thread.join();

    if (fatman1_eaten >= 10000 && fatman2_eaten >= 10000 && fatman3_eaten >= 10000) {
        all_fatmen_burst = true;
    }

    cook_thread.join();

    std::cout << "\nИтоги:" << std::endl;
    std::cout << "Толстяк 1 съел: " << fatman1_eaten << std::endl;
    std::cout << "Толстяк 2 съел: " << fatman2_eaten << std::endl;
    std::cout << "Толстяк 3 съел: " << fatman3_eaten << std::endl;
    std::cout << "Осталось в тарелках: " << dish1 << ", " << dish2 << ", " << dish3 << std::endl;

    std::cout << "Результат:";
    if (cook_fired) {
        std::cout << "1) Кука уволили" << std::endl;
    }
    else if (all_fatmen_burst) {
        std::cout << "2) Кук не получил зарплату (все толстяки лопнули)" << std::endl;
    }
    else {
        std::cout << "3) Кук уволился сам" << std::endl;
    }

    return 0;
}