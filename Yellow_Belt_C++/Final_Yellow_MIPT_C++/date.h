#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <memory>

struct Date {
	int day;
	int month;
	int year;

	Date();

	Date(const std::string& input);
	Date(int year, int month, int day);

	std::string str() const;

};

bool operator <(const Date& d1, const Date& d2);
bool operator >(const Date& d1, const Date& d2);
bool operator ==(const Date& d1, const Date& d2);
bool operator !=(const Date& d1, const Date& d2);
bool operator <=(const Date& d1, const Date& d2);
bool operator >=(const Date& d1, const Date& d2);

std::ostream& operator<<(std::ostream& out, const Date& date);

Date ParseDate(std::istream& is);