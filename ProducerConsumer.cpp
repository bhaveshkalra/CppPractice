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
std::atomic<int> counter(0); //lock free atomic counter is more thread safe counting than normal int count and helps to avoid race conditions
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
    Singleton2& s = Singleton2::getInstance();
    return 0;
}

//Thread safe singleton pattern in c++ with lazy init and destructor,
//used for logging config managers and hardware control modules, no mutex needed for thread safe
class Singleton2 {
    private:
        Singleton2() {cout<<"Instance Created"<<endl;}
        ~Singleton2() {cout<<"Instance Destroyed"<<endl;}
    
    public:
        Singleton2(const Singleton2&) = delete;
        Singleton2& operator = (const Singleton2&) = delete;

        static Singleton2& getInstance() {
            static Singleton2 instance;//thread safe c++11 and above
            return instance;
        }
};


//Thread-safe singleton pattern
mutex Singleton::mtx;
Singleton* Singleton::instance = nullptr;
class Singleton {
    private:
        static mutex mtx;
        static Singleton* instance;
        Singleton(){}
    public:
        Singleton(const Singleton&) = delete;
        Singleton& operator = (const Singleton&) = delete;
        static Singleton* getInstance() {
            lock_guard<mutex> lock(mtx);
            if(!instance) {
                instance = new Singleton();
            }
            return instance;
        }
};

