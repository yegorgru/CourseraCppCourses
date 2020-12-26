#include <numeric>
#include <vector>
#include <string>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
using namespace std;

template <typename T>
class Synchronized {
public:
    explicit Synchronized(T initial = T()):value(initial){}

    struct Access {
        T& ref_to_value;
        lock_guard<mutex> guard;
    };

    Access GetAccess() {
        return Access{value,lock_guard<mutex>(m)};
    }
private:
    T value;
    mutex m;
};