#include <iostream>
#include <semaphore>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>

using namespace std;

//Producer conusmer buffer
queue<int> buffer;
mutex mtx;
condition_variable cv;
const int MAX_SIZE = 5;

void producer(int id) {
    for (int i = 0; i < 10; i++) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock,[]{return buffer.size() < MAX_SIZE;});
        buffer.push(i);
        cout << "Produced i =" << i << endl;
        lock.unlock();
        cv.notify_all();
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

void consumer() {
    for (int i = 0 ;i < 30 ;i++) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] {
            return !buffer.empty();
        });
        cout << "Consumed:" << buffer.front() << endl;
        buffer.pop();
        lock.unlock();
        cv.notify_all();
        this_thread::sleep_for(chrono::milliseconds(150));
    }
}


int main() {
    thread p1(producer);
    thread p2(producer);
    thread p3(producer);
    thread c1(consumer);
    p1.join();p2.join();p3.join();c1.join();
    Singleton& s = Singleton::getInstance();
}


//Thread safe singleton pattern in c++ with lazy init and destructor,
//used for logging config managers and hardware control modules,no mutex needed for thread safe
class Singleton {
    private:
        Singleton() {cout<<"Instance Created"<<endl;}
        ~Singleton() {cout<<"Instance Destroyed"<<endl;}
    
    public:
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;

        static Singleton& getInstance() {
            static Singleton instance;//thread safe c++11
            return instance;
        }
};