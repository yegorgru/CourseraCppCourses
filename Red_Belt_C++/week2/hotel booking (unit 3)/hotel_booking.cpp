#include <map>
#include <set>
#include <string>
#include <utility>
#include <cstdint>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <deque>
#include <vector>

using namespace std;

struct Node {
	int64_t time=0;
	string hotel="";
	int32_t client=0;
	int32_t room_count=0;
};

bool operator < (const Node& lhs, const Node& rhs) {
	return lhs.time < rhs.time;
}

class HostelManager {
public:
	void Book(int64_t time, const string& hotel, int32_t client, int32_t room_count) {
		last_time = time;
		events.push_back({ time,hotel,client,room_count });

		Node node = { time - 86400,"",0,0 };

		auto it = upper_bound(begin(events), end(events), node);

		for (auto i = begin(events); i != it; i++) {
			ROOMS_COUNT[i->hotel] -= i->room_count;
			if (CLIENTS[i->hotel][i->client] <= time - 86400) {
				ACTUAL_CLIENTS[i->hotel].erase(i->client);
			}
		}
		ROOMS_COUNT[hotel] += room_count;

		events.erase(begin(events), it);

		CLIENTS[hotel][client] = time;
		ACTUAL_CLIENTS[hotel].insert(client);
	}

	size_t Client(const string& hotel){
		try
		{
			return ACTUAL_CLIENTS[hotel].size();
		}
		catch (const std::exception&)
		{
			return 0;
		}

	}

	int64_t Room(const string& hotel) const {
		try
		{
			return ROOMS_COUNT.at(hotel);
		}
		catch (const std::exception&)
		{
			return 0;
		}
	}

private:

	int64_t last_time = 0;

	deque<Node>events;
	map<string, int64_t> ROOMS_COUNT;
	map<string, map<int32_t,int64_t>> CLIENTS;
	map<string, set<int32_t>>ACTUAL_CLIENTS;
};

int main() {
	// Для ускорения чтения данных отключается синхронизация
	// cin и cout с stdio,
	// а также выполняется отвязка cin от cout
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	HostelManager manager;

	int query_count;
	cin >> query_count;

	for (int query_id = 0; query_id < query_count; ++query_id) {
		string query_type;
		cin >> query_type;

		if (query_type == "BOOK") {
			int64_t time;
			string hotel;
			int32_t client;
			int room;
			cin >> time >> hotel >> client >> room;
			manager.Book(time, hotel, client, room);
		}
		else if (query_type == "CLIENTS") {
			string hotel;
			cin >> hotel;
			cout << manager.Client(hotel) << "\n";
		}
		else if (query_type == "ROOMS") {
			string hotel;
			cin >> hotel;
			cout << manager.Room(hotel) << "\n";
		}
	}

	return 0;
}