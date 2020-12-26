#pragma once

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <utility>
#include <memory>
#include <string>
#include <sstream>
#include <set>

#include "date.h"

class Database {
public:
	void Add(Date date, const std::string& event);
	void Print(std::ostream& out) const;

	template <typename T>
	int RemoveIf(T predicate) {
		int to_return = 0;
		std::vector<Date>empties;
		for (auto& i : DATAv) {
			auto it = stable_partition(begin(i.second), end(i.second), [i, predicate](std::string x) {
				return !(predicate(i.first, x));
				});
			to_return += end(i.second) - it;
			for (auto j = it; j != end(i.second); ++j) {
				DATAs[i.first].erase(*j);
			}
			i.second.erase(it, end(i.second));
			if (i.second.size() == 0) {
				empties.push_back(i.first);
			}
		}
		for (const auto& i : empties) {
			DATAv.erase(i);
			DATAs.erase(i);
		}
		return to_return;
	}

	template <typename T>
	std::vector<std::string> FindIf(T predicate) const
	{
		std::vector<std::string> to_return;
		for (const auto& i : DATAv) {
			for (const auto& j : i.second) {
				if (predicate(i.first, j)) {
					to_return.push_back(i.first.str()+' '+j);
				}
			}
		}
		return to_return;
	}

	std::string Last(Date d) const;

private:
	std::map<Date, std::vector<std::string>> DATAv;
	std::map<Date, std::set<std::string>> DATAs;
};
