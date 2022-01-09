#include <iomanip>
#include <iostream>
#include <vector>
#include <utility>
#include <map>

using namespace std;

class ReadingManager {
public:
    ReadingManager()
        : mPagesInfo(1001, { 0, 0.0 }) {}

    void Read(int user_id, int page_count) {
        auto found = mUsersInfo.find(user_id);
        if (found != mUsersInfo.end()) {
            mPagesInfo[found->second].first = max(0, mPagesInfo[found->second].first - 1);
            found->second = page_count;
        }
        else {
            mUsersInfo[user_id] = page_count;
        }
        ++mPagesInfo[page_count].first;
        double current = 0.0;
        for (size_t i = 0; i < 1001; ++i) {
            mPagesInfo[i].second = current / (mUsersInfo.size() - 1);
            current += mPagesInfo[i].first;
        }
    }

    double Cheer(int user_id) const {
        if (mUsersInfo.find(user_id) == mUsersInfo.end()) {
            return 0;
        }
        if (mUsersInfo.size() == 1) {
            return 1;
        }
        return mPagesInfo.at(mUsersInfo.at(user_id)).second;
    }

private:
    static const int MAX_USER_COUNT_ = 100'000;

    vector<pair<int, double>> mPagesInfo;
    map<int, int> mUsersInfo;
};


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ReadingManager manager;

    int query_count;
    cin >> query_count;

    for (int query_id = 0; query_id < query_count; ++query_id) {
        string query_type;
        cin >> query_type;
        int user_id;
        cin >> user_id;

        if (query_type == "READ") {
            int page_count;
            cin >> page_count;
            manager.Read(user_id, page_count);
        }
        else if (query_type == "CHEER") {
            cout << setprecision(6) << manager.Cheer(user_id) << "\n";
        }
    }

    return 0;
}