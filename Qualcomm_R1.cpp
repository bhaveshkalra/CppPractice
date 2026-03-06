#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_set>
#include<unordered_map>
#include<set>

using namespace std;

//Non-Copyable but Moveable Class
class Base {
private:
    int* data;

public:
    Base(int c) : data(new int(c)) {
        cout << "Base Constructor\n";
    }

    //disable copy
    Base(const Base&) = delete;
    Base& operator=(const Base&) = delete;

    //move constructor
    Base(Base&& other) noexcept {
        cout << "Move Constructor\n";
        data = other.data;
        other.data = nullptr;
    }

    //move assignment
    Base& operator=(Base&& other) noexcept {
        cout << "Move Assignment\n";
        if (this != &other) {
            delete data;
            data = other.data;
            other.data = nullptr;
        }
        return *this;
    }

    ~Base() {
        delete data;
        cout << "Base Destructor\n";
    }
};

//Smart Pointers
class BaseTile {
public:
    virtual ~BaseTile() = default;
};

class WifiTile : public BaseTile {
public:
    WifiTile() { cout << "WifiTile Created\n"; }
    ~WifiTile() { cout << "WifiTile Destroyed\n"; }
};

void smartPointers() {
    // unique_ptr
    unique_ptr<BaseTile> w = make_unique<WifiTile>();

    // shared_ptr
    shared_ptr<int> p1 = make_shared<int>(5);
    shared_ptr<int> p2 = p1;     // ref count = 2
    (*p2)++;

    cout << "Shared value: " << *p1 << endl;

    // weak_ptr
    weak_ptr<int> w1 = p1;

    if (auto sp = w1.lock()) {
        cout << "Weak_ptr locked value: " << *sp << endl;
    }
}

//Odd/Even Printing using Threads
int countVal = 1;
mutex mtx;
condition_variable cv;

void printOdd() {
    while (true) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] { return countVal % 2 == 1 || countVal > 100; });
        if (countVal > 100) break;
        cout << "Odd Thread: " << countVal++ << endl;
        cv.notify_one();
    }
}

void printEven() {
    while (true) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] { return countVal % 2 == 0 || countVal > 100; });
        if (countVal > 100) break;
        cout << "Even Thread: " << countVal++ << endl;
        cv.notify_one();
    }
}

//Linked List Remove Duplicates
struct Node {
    int data;
    Node* next;
    Node(int x) : data(x), next(nullptr) {}
};

void printList(Node* head) {
    while (head) {
        cout << head->data << " -> ";
        head = head->next;
    }
    cout << "NULL\n";
}

void removeDuplicatesKeepOne(Node** head) {
    if (!head || !(*head)) return;
    unordered_set<int> seen;
    Node* curr = *head;
    Node* prev = nullptr;
    while (curr) {
        if (seen.count(curr->data)) {
            // duplicate → delete
            prev->next = curr->next;
            Node* temp = curr;
            curr = curr->next;
            delete temp;
        }
        else {
            seen.insert(curr->data);
            prev = curr;
            curr = curr->next;
        }
    }
}

unordered_map<int, int> mp;
set<int> deleteKeys;

void removeDuplicates(struct Node** head) {
    mp.clear();
    deleteKeys.clear();
    struct Node* curr = *head;
    while (curr != NULL) {
        if (mp[curr->data] >= 1) {
            deleteKeys.insert(curr->data);
        }
        mp[curr->data]++;
        curr = curr->next;
    }
    for (auto key : deleteKeys) {
        removeAllNodes(head, key);
    }
    return;
}

void removeAllNodes(struct Node** head, int key) {
    int count = mp[key];
    int deleteCount = count;
    struct Node* temp = *head, * prev = NULL;
    while (temp != NULL && deleteCount > 0) {
        while (temp->next != NULL && !temp->data == key) {
            prev = temp; temp = temp->next;
        }
        if (temp->data == key) {
            if (deleteCount != count) {
                prev->next = temp->next; //1->2->2->2(temp) 
                free(temp);
            }
            else {
                prev = temp;
            } deleteCount--;// temp = prev->next; 
        }
    }
    return;
}


int main() {
    Base b1(10);
    Base b2 = std::move(b1);

    smartPointers();

    thread t1(printOdd);
    thread t2(printEven);
    t1.join(); t2.join();

    // 1 -> 5 -> 5 -> 5 -> 3 -> 2 -> 6 -> 1
    Node* head = new Node(1);
    head->next = new Node(5);
    head->next->next = new Node(5);
    head->next->next->next = new Node(5);
    head->next->next->next->next = new Node(3);
    head->next->next->next->next->next = new Node(2);
    head->next->next->next->next->next->next = new Node(6);
    head->next->next->next->next->next->next->next = new Node(1);

    cout << "Original List:\n";
    printList(head);
    removeDuplicatesKeepOne(&head);

    cout << "After Removing Duplicates (Keep One):\n";
    printList(head);
    return 0;
}


//1->5->5->5->3->2->6->1->null 
//1->5->3->2->6->NULL 
