 #include<iostream>
 #include<thread>
 #include <mutex>
#include <queue>
#include <condition_variable>


 using namespace std;
 //interface
class Drawable {
public:
    virtual void draw() = 0; // pure virtual function
};

class Animal {//abstract class
public:
    virtual void show() { std::cout << "Animal\n"; }
    //virtual function
    virtual void speak() = 0;  //pure virtual function

    ~Animal() {
        std::cout << "Animal Destructor\n";
    }
};

class Dog : public Animal {
public:
    void show() override { std::cout << "Dog\n"; }
    void speak() override { std::cout << "Woof!\n"; }
     ~Dog() {
        std::cout << "Dog Destructor\n";
    }
};

int main() {
    Animal* A = new Dog();
    A->show(); // Output: Dog (Dynamic dispatch)
    A->speak(); // Output: Woof
    delete A; //Output: Dog Destructor , Animal Destructor
    
    //Dynamic dispatch is When a virtual function is called via a base class pointer, 
    //the decision about which derived function to run happens at runtime, not compile time.
    //Here vptr A points to Dog class vtable, so Dog show function is called
}

/*
L-value	Has a location in memory	int x = 10; — x is an L-value
R-value	Temporary, doesn't persist	x = 5 + 6; — 5 + 6 is an R-value
Copy constructor is called for L-values (obj2 = obj1)
Move constructor is called for R-values (obj2 = getTempObj())
*/

class A {
    int* data;

    public:
        A() { data = new int[10];}
        //copy constructor - Deep copy logic
        A(const A& other) {
            data = new int[10];
            std::copy(other.data, other.data + 10, data);
        }
        //move constructor - Take ownership of other's data
        A(A&& other) noexcept {
            data = other.data;
            other.data = nullptr;
        }
        ~A() {
            delete[] data;
        }
};


//Deadlock Scenario in threads
std::mutex m1, m2;

void threadA() {
    std::lock_guard<std::mutex> lock1(m1);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::lock_guard<std::mutex> lock2(m2);
}

void threadB() {
    std::lock_guard<std::mutex> lock2(m2);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::lock_guard<std::mutex> lock1(m1);
}

void safeThreads1() {
    std::scoped_lock lock(m1, m2);  // Deadlock-safe
}

void safeThreads2() {
    std::lock(m1, m2);
    std::lock_guard<std::mutex> lock1(m1, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(m2, std::adopt_lock);
    std::cout << "Task 1 completed\n";
}