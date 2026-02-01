//Implement reference counting with shared pointer
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

volatile bool data_ready_flag = false;
//interrupt occurs
void interrupt_handler() {
    data_ready_flag = true; 
}

int add(int a, int b) { return a + b; }

int main() {
    //unique pointer - sole ownership
    std::unique_ptr<int> ptr = std::make_unique<int>(10);

    //shared pointer-> reference counting
    std::shared_ptr<int> sp1 = std::make_shared<int>(5);
    std::shared_ptr<int> sp2 = sp1;  // count++

    //weak pointer
    std::weak_ptr<int> wp = sp1;

    volatile uint32_t* hardware_status_register = (volatile uint32_t*)0x12345000;
    //Tells compiler not to optimize reads/writes (used in hardware register reads, ISR communication).
    //Marking the pointer as volatile ensures the program always reads the current status from memory. 
    //for real-time ISR communication with main loop
    interrupt_handler();
    while (true) {
        if (data_ready_flag) {
            // Process the data, always checking the current value in memory
            //process_data();
            data_ready_flag = false; 
        }
    }
    int x = 5;
    int *p = &x;
    int **pp = &p;
    //Function pointers
    int (*fptr)(int, int) = &add;

    //Dangling pointer
    int* p;
    {
        int x = 5;
        p = &x;
    } // p is now dangling
}
