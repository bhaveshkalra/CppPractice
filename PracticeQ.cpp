//Macros are expanded by the preprocessor and dont have type safety.
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <atomic>

using namespace std;

#define SQUARE(x) ((x) * (x))
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
#define MIN2(a, b) ((a) < (b) ? (a) : (b))
#define MIN3NUMS(a, b, c) (MIN2(MIN2(a, b), c))
#undef SQUARE
#define DEBUG

//Multiple produers consumer problem with atomic counter : Mulitple producers push updates,
//single consumer reads them
//Thread 1 increments a counter up to a certain limit.
//Thread 2 waits and prints the counter every time it changes.
#define MAX_PRODUCERS 3
int counter = 0;
const int MAX_COUNT = 10;

atomic<int> acounter(0);
mutex mtx;
condition_variable cv;//allows thread to wait and signal for changes
bool updated = false;//flag for synchromization between producer and consumer

void incrementThread() { //Producer problem
    for (int i = 1; i <= MAX_COUNT; ++i) {
        {
            lock_guard<mutex> lock(mtx); //lock guard locks the mutex in safe way
            counter = i;
            updated = true;
        }
        cv.notify_one();  //signal print thread
        this_thread::sleep_for(chrono::milliseconds(100));  //simulate work
    }
}

void printThread() {//Consuemr problem
    while (true) {
        unique_lock<mutex> lock(mtx); //for using cv.wait()
        cv.wait(lock, [] { return updated; });  //wait until counter is updated to true
        cout << "Counter: " << counter << endl;
        updated = false;

        if (counter == MAX_COUNT) break;
    }
}


void producer(int id) {
    while(true) {
        int prev = acounter.load();
        if (prev >= MAX_COUNT) break;
        int newVal = ++acounter;
        {
            unique_lock<mutex> lock(mtx);
            updated = true;
            cout << "[Producer : id=>" << id <<"] incremented to" << newVal <<endl;
        }
        cv.notify_one();
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}


void consumer() {
    while(true) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] {return updated;});
        updated = false;
        cout << ">>>Consumer : Counter = " << acounter.load() << endl;
        if (acounter.load() >= MAX_COUNT) break;
    }
}


int main() {
    #ifdef DEBUG
        cout << "Debug mode\n";
    #endif
    //thread t1(incrementThread);
    //thread t2(printThread);
    vector<thread> producers;
    thread tConumser(consumer);
    for (int i = 1; i <= MAX_PRODUCERS ; i++) {
        producers.emplace_back(producer,i);
    }
    for (auto &tp : producers) {
        tp.join();
    }
    tConumser.join();
}


//COPY and MOVE constructor 
class MyDatas {
    char* strdata;
    int * data;
    MyDatas() {
        data = new int[10];
        strdata = new char[10];
    }
    MyDatas(const char* str) {
        strdata = new char[strlen(str) + 1];
        strcpy(strdata, str);
    }
    MyDatas(const MyDatas& other) {
        data = new int[10];
        std::copy(other.data, other.data + 10, data);
    }
    //move
    MyDatas(MyDatas&& other) noexcept {
        strdata = other.strdata;
        other.strdata = nullptr;
    }
    MyDatas(MyDatas&& other) noexcept {
        data = std::move(other.data);
        /*
        data = other.data;
        other.data = nullptr;
         */
    }
    ~MyDatas() {
        delete[] data;delete[] strdata;
    }
};


class SpinLock {
    atomic_flag flag = ATOMIC_FLAG_INIT;
    public:
        void lock() {
            while (flag.test_and_set(memory_order_acquire)) {}//busy and wait
        }
        void unlock() {
            flag.clear(memory_order_release);
        }
};

SpinLock spin;
int counter2 = 0;

void increment() {
    for (int i = 0;i<1000;i++) {
        spin.lock();
        ++counter2;
        spin.unlock();
    }
}