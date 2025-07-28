#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
using namespace std;

#define MAX_THREADS 3
#define TASKS_PER_PRODUCER 5

std::condition_variable cv;
std::mutex mtx;
std::atomic<int> counter(0);
bool ready = false;

void producer(int id) {
    for (int i = 0; i < TASKS_PER_PRODUCER; ++i) {
        {
            std::unique_lock<std::mutex> lock(mtx);
            ++counter;
            cout << "Producer " << id << " added task. Counter: " << counter.load() << "\n";
            ready = true;
        }
        cv.notify_one();  // Notify consumer
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}


void consumer() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return ready || counter > 0; });
        while (counter > 0) {
            cout << "Consumer processed task. Remaining: " << counter.load() - 1 << "\n";
            --counter;
        }
        ready = false;
        if (counter == 0 && !ready)
            break;
    }
}


int main() {
    std::vector<std::thread> producers;
    for (int i = 1; i <= MAX_THREADS; ++i)
        producers.emplace_back(producer, i);
    std::thread t_consumer(consumer);
    for (auto &t : producers)
        t.join();
    t_consumer.join();
    cout << "All tasks processed.\n";
    return 0;
}


//Thread-safe singleton pattern
class Singleton {
    private:
        Singleton(){}
        static mutex mtx;
        static Singleton* instance;

    public:
        static Singleton* getInstance() {
            lock_guard<mutex> lock(mtx);
            if (!instance) {
                instance = new Singleton();
            }
            return instance;
        }
};

mutex Singleton::mtx;
Singleton* Singleton::instance = nullptr;
