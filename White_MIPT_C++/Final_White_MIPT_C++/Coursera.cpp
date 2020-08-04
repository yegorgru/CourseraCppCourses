#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <set>
#include <iomanip>
#include <exception>
using namespace std;

struct Date {
	int day;
	int month;
	int year;

	Date(const string& input) {
		stringstream in;
		in << input;
		if (!(in >> year)) {
			throw invalid_argument("Wrong date format: "+input);
		}
		if (in.peek() == '-') {
			in.ignore();
		}
		else {
			throw invalid_argument("Wrong date format: " + input);
		}
		if (!(in >> month)) {
			throw invalid_argument("Wrong date format: " + input);
		}
		if (in.peek() == '-') {
			in.ignore();
		}
		else {
			throw invalid_argument("Wrong date format: " + input);
		}
		if (!(in >> day)) {
			throw invalid_argument("Wrong date format: " + input);
		}
		if (month < 1 || month>12) {
			throw invalid_argument("Month value is invalid: " + to_string(month));
		}
		else if (day < 1 || day>31) {
			throw invalid_argument("Day value is invalid: " + to_string(day));
		}
	}
};

bool operator <(const Date& d1, const Date& d2) {
	if (d1.year == d2.year) {
		if (d1.month == d2.month) {
			return d1.day < d2.day;
		}
		else {
			return d1.month < d2.month;
		}
	}
	else {
		return d1.year < d2.year;
	}
}

ostream& operator<<(ostream& out,const Date& date) {
	out <<setfill('0')<<setw(4)<< date.year  << '-' << setw(2) << date.month << '-' << setw(2) << date.day;
	return out;
}

int main() {
	try
	{
		string command;
		map<Date, set<string>>DATA;
		while (getline(cin, command)) {
			if (command != "") {
				stringstream comm;
				comm << command;
				string main_command;
				comm >> main_command;
				if (main_command == "Add") {
					string for_date;
					comm >> for_date;
					if (!(for_date[for_date.size() - 1] >='0' && for_date[for_date.size() - 1] <= '9')){
						cout << "Wrong date format: " << for_date << endl;
						return 1;
					}
					Date new_date(for_date);
					string new_event;
					if (comm.peek() == ' ') {
						comm.ignore();
					}
					else {
						cout << "Wrong date format: " << new_date << comm.peek()<<endl;
						return 1;
					}
					getline(comm, new_event);
					DATA[new_date].insert(new_event);
				}
				else if (main_command == "Del") {
					string for_date;
					comm >> for_date;
					if (!(for_date[for_date.size() - 1] >= '0' && for_date[for_date.size() - 1] <= '9')) {
						cout << "Wrong date format: " << for_date << endl;
						return 1;
					}
					Date date(for_date);
					string event;
					getline(comm, event);
					if (event == "") {
						if (DATA.count(date) == 0) {
							cout << "Deleted 0 events" << endl;
						}
						else {
							cout << "Deleted " << DATA[date].size() << " events" << endl;
							DATA.erase(date);
						}
					}
					else {
						if (event[0] != ' ') {
							cout << "Wrong date format: " << date << event[0] << endl;
						}
						else {
							event.erase(event.begin());
						}
						if (DATA[date].count(event) == 1) {
							DATA[date].erase(event);
							cout << "Deleted successfully" << endl;
						}
						else {
							cout << "Event not found" << endl;
						}
					}
				}
				else if (main_command == "Find") {
					string for_date;
					comm >> for_date;
					if (!(for_date[for_date.size() - 1] >= '0' && for_date[for_date.size() - 1] <= '9')) {
						cout << "Wrong date format: " << for_date << endl;
						return 1;
					}
					Date date(for_date);
					for (const auto& i : DATA[date]) {
						cout << i << endl;
					}
				}
				else if (main_command == "Print") {
					for (const auto& i : DATA) {
						for (const auto& j : i.second) {
							cout << i.first << ' ' << j << endl;
						}
					}
				}
				else {
					throw invalid_argument("Unknown command: " + main_command);
					return 1;
				}
			}
		}
	}
	catch (const std::exception& ex)
	{
		cout << ex.what() << endl;
	}
}