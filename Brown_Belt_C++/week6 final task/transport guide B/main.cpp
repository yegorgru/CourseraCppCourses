#include <string>
#include <iostream>
#include <sstream>
#include <set>
#include <vector>
#include <optional>
#include <algorithm>
#include <string_view>
#include <cmath>

#include <fstream>

struct Stop {
    std::string mName;
    mutable std::set<std::string> mBuses;
    double mLatitude;
    double mLongitude;
};

bool operator<(const Stop& fk, const std::string& lk) { return fk.mName < lk; }
bool operator<(const std::string& lk, const Stop& fk) { return lk < fk.mName; }
bool operator<(const Stop& fk1, const Stop& fk2) { return fk1.mName < fk2.mName; }

struct Bus {
    std::string mName;
    mutable std::vector<std::string> mStops;
    //BusType mType;
    //size_t mStops;
    mutable size_t mUniqueStops;
    mutable double mLength;
};

bool operator<(const Bus& fk, const std::string& lk) { return fk.mName < lk; }
bool operator<(const std::string& lk, const Bus& fk) { return lk < fk.mName; }
bool operator<(const Bus& fk1, const Bus& fk2) { return fk1.mName < fk2.mName; }

class TransportGuide {
public:
    void addStop(std::string&& name, double latitude, double longitude) {
        mStops.insert({ std::move(name), {}, latitude, longitude });
    }

    void addBus(std::string&& name, std::vector<std::string>&& stops) {
        mBuses.insert({ std::move(name),std::move(stops), 0, 0.0 });
    }

    void init() {
        for (auto& bus : mBuses) {
            Stops::const_iterator firstStop = mStops.find(bus.mStops[0]);
            firstStop->mBuses.insert(bus.mName);
            Stops::const_iterator secondStop;
            for (size_t i = 1; i < bus.mStops.size(); ++i) {
                secondStop = mStops.find(bus.mStops[i]);
                secondStop->mBuses.insert(bus.mName);
                bus.mLength += std::acos(
                    std::sin(firstStop->mLatitude) * std::sin(secondStop->mLatitude) +
                    std::cos(firstStop->mLatitude) * std::cos(secondStop->mLatitude) *
                    std::cos(std::abs(firstStop->mLongitude - secondStop->mLongitude))
                ) * 6371000;
                firstStop = secondStop;
            }
            std::sort(bus.mStops.begin(), bus.mStops.end());
            bus.mUniqueStops = std::unique(bus.mStops.begin(), bus.mStops.end()) - bus.mStops.begin();        
        }
    }

    void printBusInfo(const std::string& name, std::ostream& out) {
        auto it = mBuses.find(name);
        if (it != mBuses.end()) {
            out << "Bus " << name << ": " << it->mStops.size() << " stops on route, " << it->mUniqueStops << " unique stops, "
                << it->mLength << " route length\n";
        }
        else {
            out << "Bus " << name << ": not found\n";
        }
    }

    void printStopInfo(const std::string& name, std::ostream& out) {
        auto it = mStops.find(name);
        if (it != mStops.end()) {
            out << "Stop " << name << ": ";
            if (it->mBuses.size() == 0) {
                out << "no buses";
            }
            else {
                out << "buses";
                for (const auto& bus : it->mBuses) {
                    out << " " << bus;
                }
            }
        }
        else {
            out << "Stop " << name << ": not found";
        }
        out << "\n";
    }
private:
    using Stops = std::set<Stop, std::less<>>;
    using Buses = std::set<Bus, std::less<>>;
    
    Stops mStops;
    Buses mBuses;
};

int main() {
    std::cout.precision(6);

    //std::ifstream in("a.txt");
    //std::streambuf* cinbuf = std::cin.rdbuf();
    //std::cin.rdbuf(in.rdbuf());

    int numberOfRequests;
    std::cin >> numberOfRequests;
    std::cin.ignore();

    TransportGuide guide;

    for (int i = 0; i < numberOfRequests; ++i) {
        std::string request;
        std::getline(std::cin, request);
        std::istringstream iss(request);

        std::string command;
        iss >> command;
        if (command == "Stop") {
            iss.ignore();
            std::string stopName;
            std::getline(iss, stopName, ':');
            double latitude, longitude;
            iss >> latitude;
            latitude = latitude * 3.1415926535 / 180;
            iss.ignore();
            iss >> longitude;
            longitude = longitude * 3.1415926535 / 180;
            guide.addStop(std::move(stopName), latitude, longitude);
        }
        else if (command == "Bus") {
            iss.ignore();
            std::string busName;
            std::getline(iss, busName, ':');
            std::string s = iss.str();
            char separator = s.find('>') == s.npos ? '-' : '>';
            std::vector<std::string> stops;
            std::string stop;
            while (iss.ignore() && std::getline(iss, stop, separator)) {
                if (!iss.eof()) {
                    stop.pop_back();
                }
                stops.emplace_back(std::move(stop));
            }
            if (separator == '-') {
                stops.reserve(stops.size() * 2);
                std::copy(std::next(stops.rbegin()), stops.rend(), std::back_inserter(stops));
            }
            guide.addBus(std::move(busName), std::move(stops));
        }
    }

    guide.init();

    std::cin >> numberOfRequests;
    std::cin.ignore();

    for (int i = 0; i < numberOfRequests; ++i) {
        std::string request;
        std::getline(std::cin, request);
        std::istringstream iss(request);

        std::string command;
        iss >> command;
        iss.ignore();
        if (command == "Bus") {
            std::string name;
            std::getline(iss, name);
            guide.printBusInfo(name, std::cout);
        }
        else if (command == "Stop") {
            std::string name;
            std::getline(iss, name);
            guide.printStopInfo(name, std::cout);
        }
    }

    return 0;
}