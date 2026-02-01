#include <iostream>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>

using namespace std;

//Producer consumer buffer
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
}

// Implement Bounded Blocking Queue
class BoundedQueue {
    queue<int> q;
    int capacity;
    mutex mtx;
    condition_variable notFull,notEmpty;
    
    public:
        BoundedQueue(int cap) : capacity(cap) {}

        void enqueue(int x) {
            unique_lock<mutex> lock(mtx);
            notFull.wait(lock,[&] {return q.size() < capacity;});
            q.push(x);
            notEmpty.notify_one();
        }

        int dequeue() {
            unique_lock<mutex> lock(mtx);
            notEmpty.wait(lock,[&] {return !q.empty();});
            int val = q.front();
            q.pop();
            notFull.notify_one();
            return val;
        }

        int size() {
            lock_guard<mutex> lock(mtx);
            return q.size();
        }
};

