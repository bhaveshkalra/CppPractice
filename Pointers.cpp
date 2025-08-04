//Implement reference counting with shared piinter
#include<iostream>
#include <queue>
#include <mutex>
#include <condition_variable>

using namespace std;
template <typename T>

class SharedPtr {
    T* ptr;
    int* refCount;

    public:
        SharedPtr(T*p = nullptr) : ptr(p), refCount(new int(1)) {}

        SharedPtr(const SharedPtr& other) {
            ptr = other.ptr;
            refCount = other.refCount;
            ++(*refCount);
        }

        SharedPtr& operator =(const SharedPtr& other) {
            if(this!=&other) {
                if(--(*refCount) == 0) {
                    delete ptr;
                    delete refCount;
                }
                ptr = other.ptr;
                refCount = other.refCount;
                ++(*refCount);
            }
        }
        ~SharedPtr() {
            if (--(*refCount) == 0) {
                delete ptr;
                delete refCount;
            }
        }

        T& operator*() {return *ptr;}
        T* operator->(){return ptr;}
};

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