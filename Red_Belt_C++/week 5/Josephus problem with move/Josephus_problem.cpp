#include "test_runner.h"

#include <cstdint>
#include <iterator>
#include <numeric>
#include <list>
#include <utility>

using namespace std;

template <typename RandomIt>
void MakeJosephusPermutation(RandomIt first, RandomIt last, uint32_t step_size) {
    list<typename RandomIt::value_type> pool;
    move(first, last, back_inserter(pool));
    size_t last_pos = 0;
    size_t cur_pos = 0;
    auto it = pool.begin();
    auto next_it = pool.begin();
    while (!pool.empty()) {
        *(first++) = move(*it);
        next_it = next(it);
        pool.erase(it);
        if (pool.empty()) {
            break;
        }
        last_pos = cur_pos;
        cur_pos = (cur_pos + step_size - 1) % pool.size();
        if (last_pos < cur_pos && pool.size() - cur_pos > cur_pos - last_pos) {
            for (size_t i = 0; i < cur_pos - last_pos; i++) {
                next_it++;
            }
        }
        else if (last_pos < cur_pos) {
            next_it = pool.end();
            for (size_t i = 0; i < pool.size() - cur_pos; i++) {
                --next_it;
            }
        }
        else if (cur_pos > last_pos - cur_pos) {
            for (size_t i = 0; i < last_pos - cur_pos; i++) {
                next_it--;
            }
        }
        else {
            next_it = pool.begin();
            for (size_t i = 0; i < cur_pos; i++) {
                next_it++;
            }
        }
        it = next_it;
    }
}