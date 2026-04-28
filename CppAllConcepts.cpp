#include<stdio.h>
#include<iostream>
#include<thread>
#include<memory>

using namespace std;

class String {
	public:
		//default constructor
		String() : res(nullptr), len(0) {}  //Initializer list initliazes only once like const members const int a, int& mem, any other class object

		//parameterized constructor
		String(const char* str) {
			len = strlen(str);
			res = new char[len + 1];
			strcpy(res, str);
		}

		//copy constructor ->const for constant string and reference to avoid unnecessary copying
		String(const String& other) {
			len = other.len;
			res = new char[len + 1];
			strcpy(res, other.res);
		}

		// move constructor
		String(String&& other) noexcept { 
			len = other.len;
			res = other.res;
			other.len = 0;
			other.res = nullptr;
		}

		//copy assignment operator
		String& operator=(const String& other) {
			if (this != &other) {
				char* temp =this->res; // store current resource
				len = other.len;
				res = new char[len + 1];
				strcpy(res, other.res);
				delete[] temp; // free old resource
			}
			return *this;
		}

		//copy and swap idiom(CAS) 
		String& operator=(String other) {
			Swap(*this, other);
			return *this; // CAS idiom
		}

		void Swap(String& str1, String& str2) {
			std::swap(str1.res, str2.res);
			std::swap(str1.len, str2.len);
		}

		unsigned int length() const {
			return len;
		}

		friend ostream& operator<<(std::ostream& out, const String& str);
		friend istream& operator>>(istream& in, String& str);

		~String() {
			if (res) {
				delete[] res;
				res = nullptr;
				len = 0;
			}
		}

	private:
		//member variables
		char* res;
		unsigned int len;
};

ostream& operator<<(ostream& out, const String& str) {
	out << str.res;
	return out;
}

istream& operator>>(istream& in, String& str) {
	in >> str.res;
	return in;
}


//Smart Pointers -> Heap memory does not gets freed up if we forget to delete it, 
// so we can use smart pointers to manage memory automatically.

template <typename T>

class uniqueptr {
	private:
		T* ptr;
	public:
		uniqueptr(T* a = nullptr) : ptr(a) {
			cout << "ctor";
		}
		uniqueptr(const uniqueptr<T>& ptr) = delete; // disable copy constructor)
		uniqueptr& operator=(const uniqueptr<T>& ptr) = delete; // disable copy assignment operator

		uniqueptr(uniqueptr<T>&& other) noexcept { // move constructor -> transfer ownership R - value reference
			ptr = other.ptr;
			other.ptr = nullptr;
			cout << "move ctor";
		}

		uniqueptr& operator=(uniqueptr<T>&& other) noexcept { // move assignment operator
			if (this != &other) {
				if (ptr) delete ptr;
				ptr = other.ptr;
				other.ptr = nullptr;
			}
			cout << "move assignment";
			return *this;
		}

		T* operator->() {
			return ptr;
		}

		T& operator*() {
			return *ptr;
		}

		T* get() {
			return ptr;
		}

		void reset(T* a = nullptr) {
			if (ptr) delete ptr; //add lock before deleting resource in case of multithreading
			ptr = a;
		}

		~uniqueptr() {
			cout << "dtor";
			if (ptr) {
				delete ptr;
				ptr = nullptr;
			}
		}

};

template <typename T>

class sharedptr {
	private:
		T* res;
		int* counter; // reference count per resource

		void incrementCounter() {
			if (counter) (*counter)++;
		}

		void decrementCounter() {
			if (counter) {
				(*counter)--;
				if (*counter == 0) {
					if (res) {
						delete res;
						delete counter;
						res = nullptr;
						counter = nullptr;
					}
				}
			}
		}

	public:
		sharedptr(T* ptr = nullptr) : res(ptr), counter(new int(1)) {	
			cout << "shared ctor";
		}

		sharedptr<T>(const sharedptr<T>& ptr) { // copy constructor
			res = ptr.res;
			counter = ptr.counter;
			incrementCounter();
			cout << "shared copy ctor";
		}

		sharedptr<T>(sharedptr<T>&& ptr) { // move constructor
			res = ptr.res;
			counter = ptr.counter;
			ptr.res = nullptr;
			ptr.counter = nullptr;
			cout << "shared move ctor";
		}

		sharedptr<T>& operator=(const sharedptr<T>& ptr) { // copy assignment operator 
			if (this != &ptr) {
				decrementCounter(); // decrement current resource count
				res = ptr.res;
				counter = ptr.counter;
				incrementCounter(); // increment new resource count
			}
			cout << "shared copy assignment";
			return *this;
		}

		sharedptr<T>& operator=(sharedptr<T>&& ptr) { // move assignment operator 
			if (this != &ptr) {
				decrementCounter(); // decrement current resource count
				res = ptr.res;
				counter = ptr.counter;
				ptr.res = nullptr;
				ptr.counter = nullptr;
			}
			cout << "shared move assignment";
			return *this;
		}

		void reset(T* ptr = nullptr) {
			decrementCounter(); // decrement current resource count
			res = ptr;
			counter = new int(1); // reset counter for new resource
		}

		int get_count() const {
			return counter ? *counter : -1;
		}

		T* operator->() {
			return res;
		}

		T& operator*() {
			return *res;
		}

		T* get() {
			return res;
		}

		~sharedptr() {
			decrementCounter(); // decrement resource count and delete if count reaches 0
			cout << "dtor";
		}
};


int main() {
	String str1;// default constructor
	String str2 = "hello"; // parameterized constructor
	String str3 = str1; // copy constructor
	str3 = str2; // copy assignment operator
	int len = str2.length(); // member function

	cout << str2 << str3 << str1; //overlaod operator<<

	cin >> str1; // overload >>

	String str5(str1); // copy constructor

	String str6 = std::move(str2); // move constructor


	//Unique Pointer
	uniqueptr<int> ptr1(new int(10));
	//uniqueptr<int> ptr2(ptr1); // error: copy constructor is deleted
	//uniqueptr<int> ptr3 = ptr1; //compilation error
	uniqueptr<int> ptr4(new int(500));
	//ptr4 = ptr3; //compilation error
	uniqueptr<int> ptr3 = std::move(ptr1); // move constructor
	ptr4 = std::move(ptr3);

	//ptr1->func();
	cout << *(ptr4);
	ptr4.get();
	ptr4.reset(new int(20));


	//Shared Pointers
	sharedptr<int> ptr11;//default
	sharedptr<int> ptr21(new int(10));//parameter
	sharedptr<int> ptr31(ptr21);//copy 
	ptr31 = ptr21; //copy assignment
	sharedptr<int> ptr4(std::move(ptr11)); // move copy constructor
	ptr21 = move(ptr31); // move copy assignment

	ptr1.reset();
	ptr1.reset(new int(5));
	
	cout << (*ptr1);
	//ptr11->func();

	ptr1.get();//raw pointer 

	ptr31.get_count(); //no. of objects pointing to same resource

	//destructors will be called automatically when objects go out of scope, freeing memory & preventing leaks.
	return 0;
}

