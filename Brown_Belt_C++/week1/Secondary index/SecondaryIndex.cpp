#include "test_runner.h"

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <utility>
#include <list>

using namespace std;

struct Record {
    string id;
    string title;
    string user;
    int timestamp;
    int karma;
};

bool operator == (const Record& r1, const Record& r2) {
    return r1.id == r2.id;
}

struct RecordHasher {
    size_t operator() (const Record& record) const {
        hash<string> hasher;
        return hasher(record.id);
    }
};

struct EraseHelper {
    multimap<int, std::list<std::pair<Record, EraseHelper>>::iterator>::iterator timestamp_it;
    multimap<int, std::list<std::pair<Record, EraseHelper>>::iterator>::iterator karma_it;
    multimap<string, std::list<std::pair<Record, EraseHelper>>::iterator>::iterator user_it;
};

class Database {
public:
    bool Put(const Record& record) {
       if (id_data.find(record.id) == id_data.end()) {
           DATA.push_back({ record,{
               multimap<int, std::list<std::pair<Record, EraseHelper>>::iterator>::iterator(),
               multimap<int, std::list<std::pair<Record, EraseHelper>>::iterator>::iterator(),
               multimap<string, std::list<std::pair<Record, EraseHelper>>::iterator>::iterator()}
               });
           
           std::list<std::pair<Record, EraseHelper>>::iterator it = DATA.end();
           it--;
           auto it1 = id_data.insert({ record.id,it });
           auto it2 = timestamp_id.insert({ record.timestamp,it });
           auto it3 = karma_id.insert({ record.karma,it });
           auto it4 = user_id.insert({ record.user,it });
           it->second.timestamp_it = it2;
           it->second.karma_it = it3;
           it->second.user_it = it4;
           return true;
        }
        else {
            return false;
        }
    }

    const Record* GetById(const string& id) const {
        auto it = id_data.find(id);
        if (it == id_data.end()) {
            return nullptr;
        }
        else {
            return &(it->second->first);
        }
    }
     
    bool Erase(const string& id) {
        auto it = id_data.find(id);
        if (it == id_data.end()) {
            return false;
        }
        else {
            timestamp_id.erase(it->second->second.timestamp_it);
            karma_id.erase(it->second->second.karma_it);
            user_id.erase(it->second->second.user_it);
            DATA.erase(it->second);
            id_data.erase(it);
            return true;
        }
    }

    template <typename Callback>
    void RangeByTimestamp(int low, int high, Callback callback) const {
        auto it = timestamp_id.lower_bound(low);
        while (it != timestamp_id.end() && it->first <= high && callback(it->second->first)) {
            it++;
        }
    }

    template <typename Callback>
    void RangeByKarma(int low, int high, Callback callback) const {
        auto it = karma_id.lower_bound(low);
        while (it != karma_id.end() && it->first <= high && callback(it->second->first)) {
            it++;
        }
    }

    template <typename Callback>
    void AllByUser(const string& user, Callback callback) const {
        auto it = user_id.lower_bound(user);
        while (it != user_id.end() && it->first == user && callback(it->second->first)) {
            it++;
        }
    }

private:
    std::list<std::pair<Record, EraseHelper>>DATA;
    multimap<string, std::list<std::pair<Record, EraseHelper>>::iterator>id_data;
    multimap<int, std::list<std::pair<Record, EraseHelper>>::iterator>timestamp_id;
    multimap<int, std::list<std::pair<Record, EraseHelper>>::iterator>karma_id;
    multimap<string, std::list<std::pair<Record, EraseHelper>>::iterator>user_id;
};

void TestRangeBoundaries() {
    const int good_karma = 1000;
    const int bad_karma = -10;

    Database db;
    db.Put({ "id1", "Hello there", "master", 1536107260, good_karma });
    db.Put({ "id2", "O>>-<", "general2", 1536107260, bad_karma });

    int count = 0;
    db.RangeByKarma(bad_karma, good_karma, [&count](const Record&) {
        ++count;
        return true;
        });

    ASSERT_EQUAL(2, count);
}

void TestSameUser() {
    Database db;
    db.Put({ "id1", "Don't sell", "master", 1536107260, 1000 });
    db.Put({ "id2", "Rethink life", "master", 1536107260, 2000 });

    int count = 0;
    db.AllByUser("master", [&count](const Record&) {
        ++count;
        return true;
        });

    ASSERT_EQUAL(2, count);
}

void TestReplacement() {
    const string final_body = "Feeling sad";

    Database db;
    db.Put({ "id", "Have a hand", "not-master", 1536107260, 10 });
    db.Erase("id");
    db.Put({ "id", final_body, "not-master", 1536107260, -10 });

    auto record = db.GetById("id");
    ASSERT(record != nullptr);
    ASSERT_EQUAL(final_body, record->title);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestRangeBoundaries);
    RUN_TEST(tr, TestSameUser);
    RUN_TEST(tr, TestReplacement);
    return 0;
}