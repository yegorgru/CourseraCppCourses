#include "test_runner.h"

#include <algorithm>
#include <future>
#include <unordered_map>
#include <vector>
#include <string>
#include <random>
#include <numeric>
#include <mutex>
#include <cstdlib>
#include <climits>
#include <cmath>
using namespace std;

template <typename K, typename V, typename Hash = std::hash<K>>
class ConcurrentMap {
public:
    using MapType = unordered_map<K, V, Hash>;

    struct WriteAccess {
        lock_guard<std::mutex> l_g;
        V& ref_to_value;
    };

    struct ReadAccess {
        lock_guard<std::mutex> l_g;
        const V& ref_to_value;
    };

    explicit ConcurrentMap(size_t bucket_count) :
        m_Mutexes(bucket_count), m_Data(bucket_count), m_Hasher(Hash{})
    {}

    WriteAccess operator[](const K& key) {
        size_t index = m_Hasher(key)% m_Mutexes.size();
        return { lock_guard(m_Mutexes[index]),m_Data[index][key] };
    }

    ReadAccess At(const K& key) const {
        size_t index = m_Hasher(key)%m_Mutexes.size();
        return { lock_guard(m_Mutexes[index]),m_Data[index].at(key) };
    }

    bool Has(const K& key) const {
        size_t index = m_Hasher(key) % m_Mutexes.size();
        return m_Data[index].count(key);
    }

    MapType BuildOrdinaryMap() const {
        MapType answer;
        for (size_t i = 0; i != m_Data.size(); i++) {
            lock_guard<mutex>lock(m_Mutexes[i]);
            for (auto it = m_Data[i].begin(); it != m_Data[i].end(); it++) {
                answer[it->first] = it->second;
            }
        }
        return answer;
    }

private:
    mutable vector<mutex>m_Mutexes;
    vector<MapType>m_Data;
    Hash m_Hasher;
};