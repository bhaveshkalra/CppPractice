#include <iostream>
#include <vector>
#include <limits>
#include <memory>

using namespace std;

#define ADD(a,b) ((a)+(b))

class Base {
public:
    static int c;

    Base() {
        cout << "Base Constructor\n";
    }

    virtual void func() = 0;

    static int getC() {
        return ++c;
    }

    virtual ~Base() {
        cout << "Base Destructor\n";
    }
};

int Base::c = 0;

class Child : public Base {
public:
    Child() {
        cout << "Child Constructor\n";
    }

    void func() override {
        cout << "Child func\n";
    }

    ~Child() {
        cout << "Child Destructor\n";
    }
};

// Safe addition
bool safeAdd(uint64_t a, uint64_t b, uint64_t& res) {
    if (a > numeric_limits<uint64_t>::max() - b)
        return true;

    res = a + b;
    return false;
}

int main() {

    int n;
    cin >> n;

    // Safe memory
    vector<int> arr(n);
    unique_ptr<int[]> arr(new int[n]);//new array

    // Polymorphism + RAII
    unique_ptr<Base> b = make_unique<Child>();
    b->func();

    uint64_t a = 100;
    uint64_t d = 200;
    uint64_t c;

    if (safeAdd(a, d, c))
        cout << "Overflow\n";
    else
        cout << "Sum: " << c << endl;

    cout << "Static c: " << Base::getC() << endl;

    int const* p1;   // data const
    int* const p2 = nullptr;  // pointer const

    // C style
    int* p3 = (int*)malloc(sizeof(int));     // uninitialized
    int* p4 = (int*)calloc(5, sizeof(int));  // zero initialized

    free(p3);
    free(p4);

    volatile int* hw_register = (int*)0xFF00;
    int val = *hw_register;
    //“Volatile ensures compiler does not cache register values during polling loops.”

    auto tile = TileFactory::createTile(TileType::WIFI);
    tile->render();

    //PlaceMent new-.>
    char buffer[sizeof(WifiTile)];
    WifiTile* t = new(buffer) WifiTile();
    //Smart pointer RAII reduced lifecycle bugs when tiles dynamically appeared/disappeared during UI updates.
}

class QuickTile {
public:
    virtual void render() = 0;
    virtual ~QuickTile() = default;
};

class WifiTile : public QuickTile {
public:
    void render() override {
        // draw wifi UI
    }
};

class BluetoothTile : public QuickTile {
public:
    void render() override {
        // draw bluetooth UI
    }
};

enum class TileType {
    WIFI,
    BLUETOOTH
};

class TileFactory {
public:
    static std::unique_ptr<QuickTile> createTile(TileType type) {

        switch (type) {
        case TileType::WIFI:
            return std::make_unique<WifiTile>();

        case TileType::BLUETOOTH:
            return std::make_unique<BluetoothTile>();

        default:
            return nullptr;
        }
    }
};
