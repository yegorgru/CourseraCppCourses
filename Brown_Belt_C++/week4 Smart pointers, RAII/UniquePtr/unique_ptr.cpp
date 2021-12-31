#include "test_runner.h"

#include <cstddef>

using namespace std;

template <typename T>
class UniquePtr {
public:
    UniquePtr() {
        mData = nullptr;
    }

    UniquePtr(T* ptr) {
        mData = ptr;
    }

    UniquePtr(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) {
        mData = other.mData;
	other.mData = nullptr;
    }

    UniquePtr& operator = (const UniquePtr&) = delete;

    UniquePtr& operator = (nullptr_t) {
        Clear();
	return *this;
    }

    UniquePtr& operator = (UniquePtr&& other) {
        Clear();
        swap(mData, other.mData);
	return *this;
    }

    ~UniquePtr() {
        Clear();
    }

    T& operator * () const {
        return *mData;
    }

    T* operator -> () const {
        return mData;
    }

    T* Release() {
        T* result = mData;
        mData = nullptr;
        return result;
    }

    void Reset(T* ptr) {
        Clear();
        mData = ptr;
    }

    void Swap(UniquePtr& other) {
        swap(mData, other.mData);
    }

    T* Get() const {
        return mData;
    }

private:
    void Clear() {
        delete mData;
        mData = nullptr;
    }

    T* mData;
};


struct Item {
    static int counter;
    int value;
    Item(int v = 0) : value(v) {
        ++counter;
    }
    Item(const Item& other) : value(other.value) {
        ++counter;
    }
    ~Item() {
        --counter;
    }
};

int Item::counter = 0;


void TestLifetime() {
    Item::counter = 0;
    {
        UniquePtr<Item> ptr(new Item);
        ASSERT_EQUAL(Item::counter, 1);

        ptr.Reset(new Item);
        ASSERT_EQUAL(Item::counter, 1);
    }
    ASSERT_EQUAL(Item::counter, 0);

    {
        UniquePtr<Item> ptr(new Item);
        ASSERT_EQUAL(Item::counter, 1);

        auto rawPtr = ptr.Release();
        ASSERT_EQUAL(Item::counter, 1);

        delete rawPtr;
        ASSERT_EQUAL(Item::counter, 0);
    }
    ASSERT_EQUAL(Item::counter, 0);
}

void TestGetters() {
    UniquePtr<Item> ptr(new Item(42));
    ASSERT_EQUAL(ptr.Get()->value, 42);
    ASSERT_EQUAL((*ptr).value, 42);
    ASSERT_EQUAL(ptr->value, 42);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestLifetime);
    RUN_TEST(tr, TestGetters);
}