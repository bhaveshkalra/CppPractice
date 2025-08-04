#include<iostream>
#include <unordered_map>
#include<vector>
#include<deque>

using namespace std;

class LRUCache {
public:
    class Node {
        public:
            int key;int val;
            Node*next,*prev;
            Node(int key,int val) {
                this->key = key;
                this->val = val;
            }
    };
    
    Node* head = new Node(-1, -1);
    Node* tail = new Node(-1, -1);
    int size;
    unordered_map<int, Node*> mp;

    LRUCache(int capacity) {
        size = capacity;
        head -> next = tail;
        tail -> prev = head;
    }

    void addNode(Node* newNode) {
        Node* temp = head->next;

        newNode->next = temp;
        newNode->prev = head;

        head->next = newNode;
        temp->prev = newNode;
    }

    void removeNode(Node* delNode) {
        Node* prevv = delNode -> prev;
        Node* nextt = delNode -> next;

        prevv ->next = nextt;
        nextt -> prev = prevv;
    }
    
    int get(int key) {
        if (mp.find(key) != mp.end()) {
            Node* resNode = mp[key];
            int ans = resNode->val;

            mp.erase(key);
            removeNode(resNode);
            addNode(resNode);

            mp[key] = head->next;
            return ans;
        }
        return -1;
    }
    
    void put(int key, int value) {
        if (mp.find(key) != mp.end()) {
            Node* curr = mp[key];
            mp.erase(key);
            removeNode(curr);
        }
        if (mp.size() == size) {
            mp.erase(tail->prev->key);
            removeNode(tail->prev);
        }
        addNode(new Node(key,value));
        mp[key] = head->next;
    }
};

vector<int> slidingWindowMaximum(vector<int>& nums,int k) { //return vector with maxium in window size k
    deque<int> dq;
    vector<int> res;

    
}

/**
 * Your LRUCache object will be instantiated and called as such:
 * LRUCache* obj = new LRUCache(capacity);
 * int param_1 = obj->get(key);
 * obj->put(key,value);
 */