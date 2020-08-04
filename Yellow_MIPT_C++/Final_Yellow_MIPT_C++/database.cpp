#include "database.h"

void Database::Add(Date date, const std::string& event) {
	if (DATAs[date].count(event) == 0) {
		DATAv[date].push_back(event);
		DATAs[date].insert(event);
	}
}

void Database::Print(std::ostream& out) const {
	for (const auto& i : DATAv) {
		for (const auto& j : i.second) {
			out << i.first << ' ' << j << std::endl;
		}
	}
}

std::string Database::Last(Date d) const
{
	auto iter_after = DATAv.upper_bound(d);
	if (iter_after != DATAv.begin()) {
		//std::stringstream ss;
		//ss << (--iter_after)->first<<' '<< (iter_after)->second[iter_after->second.size() - 1];
		return (--iter_after)->first.str() + ' ' + (iter_after)->second[iter_after->second.size() - 1];
	}
	else {
		throw std::invalid_argument("");
	}
}
