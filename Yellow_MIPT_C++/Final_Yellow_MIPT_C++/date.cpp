#include "date.h"

Date::Date()
{
	year = 0;
	month = 0;
	year = 0;
}

Date ParseDate(std::istream& is) {
	Date date;
	if (!(is >> date.year)) {
		throw std::invalid_argument("Wrong date format: ");
	}
	if (is.peek() == '-') {
		is.ignore();
	}
	else {
		throw std::invalid_argument("Wrong date format: ");
	}
	if (!(is >> date.month)) {
		throw std::invalid_argument("Wrong date format: ");
	}
	if (is.peek() == '-') {
		is.ignore();
	}
	else {
		throw std::invalid_argument("Wrong date format: ");
	}
	if (!(is >> date.day)) {
		throw std::invalid_argument("Wrong date format: ");
	}
	if (date.month < 1 || date.month>12) {
		throw std::invalid_argument("Month value is invalid: ");
	}
	else if (date.day < 1 || date.day>31) {
		throw std::invalid_argument("Day value is invalid: ");
	}
	return date;
}

Date::Date(const std::string& input) {
	std::stringstream in;
	in << input;
	if (!(in >> year)) {
		throw std::invalid_argument("Wrong date format: " + input);
	}
	if (in.peek() == '-') {
		in.ignore();
	}
	else {
		throw std::invalid_argument("Wrong date format: " + input);
	}
	if (!(in >> month)) {
		throw std::invalid_argument("Wrong date format: " + input);
	}
	if (in.peek() == '-') {
		in.ignore();
	}
	else {
		throw std::invalid_argument("Wrong date format: " + input);
	}
	if (!(in >> day)) {
		throw std::invalid_argument("Wrong date format: " + input);
	}
	if (month < 1 || month>12) {
		throw std::invalid_argument("Month value is invalid: " + std::to_string(month));
	}
	else if (day < 1 || day>31) {
		throw std::invalid_argument("Day value is invalid: " + std::to_string(day));
	}
}

Date::Date(int year, int month, int day)
{
	this->year = year;
	this->month = month;
	this->day = day;
}

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

bool operator >(const Date& d1, const Date& d2) {
	if (d1.year == d2.year) {
		if (d1.month == d2.month) {
			return d1.day > d2.day;
		}
		else {
			return d1.month > d2.month;
		}
	}
	else {
		return d1.year > d2.year;
	}
}

bool operator ==(const Date& d1, const Date& d2) {
	return d1.day == d2.day && d1.month == d2.month
		&& d1.year == d2.year;
}

bool operator !=(const Date& d1, const Date& d2) {
	return d1.day != d2.day || d1.month != d2.month
		|| d1.year != d2.year;
}

bool operator <=(const Date& d1, const Date& d2) {
	if (d1.year == d2.year) {
		if (d1.month == d2.month) {
			return d1.day <= d2.day;
		}
		else {
			return d1.month <= d2.month;
		}
	}
	else {
		return d1.year <= d2.year;
	}
}

bool operator >=(const Date& d1, const Date& d2) {
	if (d1.year == d2.year) {
		if (d1.month == d2.month) {
			return d1.day >= d2.day;
		}
		else {
			return d1.month >= d2.month;
		}
	}
	else {
		return d1.year >= d2.year;
	}
}

std::ostream& operator<<(std::ostream& out, const Date& date) {
	out << std::setfill('0') << std::setw(4) << date.year << '-' << std::setw(2) << date.month << '-' << std::setw(2) << date.day;
	return out;
}

std::string Date::str() const
{
	std::ostringstream ost;
	ost << std::setw(4) << std::setfill('0') << year << '-'
		<< std::setw(2) << month << '-' << std::setw(2) << day;
	return ost.str();
}