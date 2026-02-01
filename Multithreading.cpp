 #include<iostream>
 #include<thread>
 #include<mutex>
 #include<condition_variable>
 #include<vector>

using namespace std;

void helloWorld() {
    cout << "Hello World" << " " << this_thread::get_id() << endl;
    // some io operation is here
}

int main() {
    cout << this_thread::get_id() << endl; 

    vector<thread> threads;
    int n = thread::hardware_concurrency();
    cout << "Hardware concurrency" << " " << n << endl;

    for (int i = 0; i < 5; i++) {
        threads.push_back(thread(helloWorld));
    }

    for (int i = 0; i < 5; i++) {
        threads[i].join();
    }

    return 0;
}

//LC 1115 
//Print FooBar Alternately

class FooBar {
private:
    int n;
    condition_variable cv;
    mutex mtx;
    bool isFoo;

public:
    FooBar(int n) {
        this->n = n;
        isFoo = false;
    }

    void foo(function<void()> printFoo) { //Thread 1
        
        for (int i = 0; i < n; i++) {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [this] {return !isFoo;});
        	printFoo();
            isFoo = true;
            cv.notify_all();
        }
    }

    void bar(function<void()> printBar) { // Thread 2
        for (int i = 0; i < n; i++) {
        	unique_lock<mutex> lock(mtx);
            cv.wait(lock, [this] {return isFoo;});
        	printBar();
            isFoo = false;
            cv.notify_all();
        }
    }
};