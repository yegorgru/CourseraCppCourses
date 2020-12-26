#include "test_runner.h"

#include <forward_list>
#include <iterator>
#include <vector>
#include <algorithm>

using namespace std;

template <typename Type, typename Hasher>
class HashSet {
public:
    using BucketList = forward_list<Type>;

public:
    explicit HashSet(
        size_t num_buckets,
        const Hasher& hasher = {}
    ): hasher(hasher),buckets(num_buckets) {
        
    }

    void Add(const Type& value) {
        if (!Has(value)) {
            buckets[hasher(value) % buckets.size()].push_front(value);
        }
    }

    bool Has(const Type& value) const {
        size_t pos = hasher(value) % buckets.size();
        if (std::find(buckets[pos].begin(), buckets[pos].end(), value) != end(buckets[pos])) {
            return true;
        }
        else {
            return false;
        }
    }
    void Erase(const Type& value) {
        size_t pos = hasher(value) % buckets.size();
        buckets[pos].remove(value);
    }
    const BucketList& GetBucket(const Type& value) const {
        return buckets[hasher(value) % buckets.size()];
    }
private:
    vector<BucketList>buckets;
    Hasher hasher;
};