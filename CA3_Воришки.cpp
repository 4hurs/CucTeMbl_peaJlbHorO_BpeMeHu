#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

std::mutex m;
int coins = 100;
int Bob_coins = 0;
int dead_coins = 0;
int Tom_coins = 0;

void coin_sharing(std::string name, int& thief_coins, int& companion_coins) {
    while (true) {
        m.lock();

        if (coins <= 0) {
            m.unlock();
            break;
        }

        if (coins == 1 and thief_coins == companion_coins) {
            coins--;
            dead_coins++;
            std::cout << "Покойник взял монету" << std::endl;
            m.unlock();
            break;
        }
        else if (thief_coins <= companion_coins) {
            coins--;
            thief_coins++;

            std::cout << name << " взял монету. ";
            std::cout << "У " << name << ": " << thief_coins << ", ";
            std::cout << "У напарника: " << companion_coins << ", ";
            std::cout << "Осталось: " << coins << std::endl;
        }

        m.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    std::cout << "=== Начало дележа монет ===" << std::endl;
    std::cout << "Всего монет: " << coins << std::endl;

    std::thread bob_thread(coin_sharing, "Боб", std::ref(Bob_coins), std::ref(Tom_coins));
    std::thread tom_thread(coin_sharing, "Том", std::ref(Tom_coins), std::ref(Bob_coins));

    bob_thread.join();
    tom_thread.join();

    std::cout << "\n=== Итоги дележа ===" << std::endl;
    std::cout << "У Боба: " << Bob_coins << " монет" << std::endl;
    std::cout << "У Тома: " << Tom_coins << " монет" << std::endl;
    std::cout << "Покойнику: " << dead_coins << " монет" << std::endl;
    std::cout << "Всего монет: " << (Bob_coins + Tom_coins + dead_coins) << std::endl;

    return 0;
}