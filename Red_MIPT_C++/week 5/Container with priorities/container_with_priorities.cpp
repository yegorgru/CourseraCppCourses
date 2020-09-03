#include "test_runner.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <set>
#include <utility>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <climits>

using namespace std;

template <typename T>
struct IDD {
    typename list<T>::iterator it;
    long long number;
};

template <typename T>
class PriorityCollection {
public:

    using Id = IDD<T>;

    struct lex_compare {
        bool operator() (const IDD<T>& lhs, const IDD<T>& rhs) const {
            return lhs.number < rhs.number;
        }
    };

    Id Add(T object) {
        DATABASE.push_back(move(object));
        auto it = DATABASE.end();
        it--;
        Id ID = {it ,++max_number };
        priorities[ID] = 0;
        all_ID.insert(ID);
        DATA[0].insert(ID);
        return ID;
    }
    
    template <typename ObjInputIt, typename IdOutputIt>
    void Add(ObjInputIt range_begin, ObjInputIt range_end,
        IdOutputIt ids_begin) {
        auto cur_id = ids_begin;
        for (auto i = range_begin; i != range_end; i++) {
            Id cur = Add(move(*i));
            *cur_id++ = cur;
        }
    }

    bool IsValid(Id id) const {
        if (all_ID.find(id) != end(all_ID)) {
            return true;
        }
        else {
            return false;
        }
    }

    const T& Get(Id id) const {
        return *(id.it);
    }

    void Promote(Id id) {
        int priority = priorities[id];
        auto it = DATA[priority].find(id);
        DATA[priority + 1].insert(id);
        DATA[priority].erase(it);
        if (DATA[priority].size() == 0) {
            DATA.erase(priority);
        }
        priorities[id]++;
        //splice(DATA[priority + 1].end(),DATA[priority],id);
        //priorities[id]++;
    }

    pair<const T&, int> GetMax() const {
        int prior = DATA.rbegin()->first;
        auto en = DATA.at(prior).end();
        --en;
        return { *(en->it) ,prior};
    }

    pair<T, int> PopMax() {
        pair<T, int> answer;
        int prior = DATA.rbegin()->first;
        auto en = DATA[prior].end();
        --en;
        answer.first = move(*(en->it));
        answer.second = prior;
        DATABASE.erase(en->it);
        all_ID.erase(*en);
        priorities.erase(*en);
        DATA[prior].erase(en);
        if (DATA[prior].size() == 0) {
            DATA.erase(prior);
        }
        return answer;
    }

private:
    map<int, set<Id,lex_compare>>DATA;
    list<T>DATABASE;
    set <Id, lex_compare> all_ID;
    map<Id, int, lex_compare> priorities;
    long long max_number=LLONG_MIN;
};