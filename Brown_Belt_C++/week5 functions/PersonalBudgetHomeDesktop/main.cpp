#include <iostream>
#include <ctime>
#include <string>
#include <sstream>
#include <map>
#include <array>
#include <stdexcept>
#include <iomanip>

#include "test_runner.h"

class Date {
public:
    Date(int year, int month, int day) {
        mYear = year;
        mMonth = month;
        mDay = day;
    }

    Date(const std::string& str) {
        std::istringstream iss(str);
        iss >> mYear;
        iss.ignore();
        iss >> mMonth;
        iss.ignore();
        iss >> mDay;
    }

    bool operator<(const Date& another) const {
        if (mYear == another.mYear) {
            if (mMonth == another.mMonth) {
                return mDay < another.mDay;
            }
            return mMonth < another.mMonth;
        }
        return mYear < another.mYear;
    }

    bool operator==(const Date& another) const {
        return mYear == another.mYear && mMonth == another.mMonth && mDay == another.mDay;
    }

    bool operator<=(const Date& another) const {
        return *this < another || *this == another;
    }

    Date& operator++() {
        ++mDay;
        if(mDay > DAYS_IN_MONTH[mMonth] && !(mMonth == 2 && isLeapYear(mYear) && mDay == 29)) {
            ++mMonth;
            mDay = 1;
            if (mMonth == 13) {
                ++mYear;
                mMonth = 1;
            }
        }
        return *this;
    }

    time_t AsTimestamp() const {
        std::tm t;
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_mday = mDay;
        t.tm_mon = mMonth - 1;
        t.tm_year = mYear - 1900;
        t.tm_isdst = 0;
        return mktime(&t);
    }

    static int ComputeDaysDiff(const Date& dateFrom, const Date& dateTo) {
        const time_t timestampTo = dateTo.AsTimestamp();
        const time_t timestampFrom = dateFrom.AsTimestamp();
        static const int SECONDS_IN_DAY = 60 * 60 * 24;
        return (timestampTo - timestampFrom) / SECONDS_IN_DAY;
    }

    static bool isLeapYear(int year) {
        return year % 4 == 0 && !(year % 100 == 0) || year % 400 == 0;
    }
private:
    inline static std::array<int, 13> DAYS_IN_MONTH = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    int mYear;
    int mMonth;
    int mDay;
};

class PersonalBudgetManager {
public:
    double computeIncome(const Date& dateFrom, const Date& dateTo) {
        if (dateTo < dateFrom) {
            throw std::runtime_error("date to is less than second");
        }
        auto lowerIt = mBudget.lower_bound(dateFrom);
        auto upperIt = mBudget.upper_bound(dateTo);
        double income = 0.0;
        for (auto it = lowerIt; it != upperIt; ++it) {
            income += it->second;
        }
        return income;
    }

    void earn(const Date& dateFrom, const Date& dateTo, double value) {
        if (dateTo < dateFrom) {
            throw std::runtime_error("date to is less than second");
        }
        double dayIncome = value / (static_cast<double>(Date::ComputeDaysDiff(dateFrom, dateTo)) + 1);
        for (Date date = dateFrom; date <= dateTo; ++date) {
            mBudget[date] += dayIncome;
        }
    }

    void payTax(const Date& dateFrom, const Date& dateTo) {
        if (dateTo < dateFrom) {
            throw std::runtime_error("date to is less than second");
        }
        auto lowerIt = mBudget.lower_bound(dateFrom);
        auto upperIt = mBudget.upper_bound(dateTo);
        for (auto it = lowerIt; it != upperIt; ++it) {
            it->second *= 0.87;
        }
    }
private:
    std::map<Date, double> mBudget;
};

std::pair<Date, Date> readDates() {
    std::string str;
    std::cin >> str;
    Date first(str);
    std::cin >> str;
    Date second(str);
    return { first, second };
}

int main() {
    int Q;
    std::cin >> Q;
    PersonalBudgetManager manager;
    for (int i = 0; i < Q; ++i) {
        std::string command;
        std::cin >> command;
        if (command == "ComputeIncome") {
            auto dates = readDates();
            std::cout << std::setprecision(25) << std::fixed << manager.computeIncome(dates.first, dates.second) << std::endl;
        }
        else if (command == "Earn") {
            auto dates = readDates();
            double value;
            std::cin >> value;
            manager.earn(dates.first, dates.second, value);
        }
        else if(command == "PayTax"){
            auto dates = readDates();
            manager.payTax(dates.first, dates.second);
        }
        else {
            throw std::runtime_error("unknown command");
        }
    }
}