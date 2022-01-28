#include <string>
#include <iostream>
#include <sstream>
#include <set>
#include <vector>
#include <optional>
#include <algorithm>
#include <string_view>
#include <cmath>
#include <map>

#include <fstream>

struct StopInfo {
    std::set<std::string> mBuses;
    double mLatitude;
    double mLongitude;
};

struct BusInfo {
    std::vector<std::string> mStops;
    size_t mUniqueStops;
    double mDistance;
    double mCurvature;
};

class TransportGuide {
public:
    void addStop(std::string&& name, std::map<std::string, double>&& stopDistances, double latitude, double longitude) {
        for (auto& stop : stopDistances) {
            mStopDistances[{name, stop.first}] = stop.second;
            if (mStopDistances.find({ stop.first, name }) == mStopDistances.end()) {
                mStopDistances[{ stop.first, name }] = stop.second;
            }
        }
        mStops.emplace(std::move(name), StopInfo{ {}, latitude, longitude });
    }

    void addBus(std::string&& name, std::vector<std::string>&& stops) {
        mBuses.emplace(std::move(name), BusInfo{ std::move(stops), 0, 0.0, 0.0 });
    }

    void init() {
        for (auto& [busName, busInfo] : mBuses) {
            Stops::iterator firstStop = mStops.find(busInfo.mStops[0]);
            firstStop->second.mBuses.insert(busName);
            Stops::iterator secondStop;
            double geographicDistance = 0.0;
            for (size_t i = 1; i < busInfo.mStops.size(); ++i) {
                secondStop = mStops.find(busInfo.mStops[i]);
                secondStop->second.mBuses.insert(busName);
                double nextGeographicPiece = std::acos(
                    std::sin(firstStop->second.mLatitude) * std::sin(secondStop->second.mLatitude) +
                    std::cos(firstStop->second.mLatitude) * std::cos(secondStop->second.mLatitude) *
                    std::cos(std::abs(firstStop->second.mLongitude - secondStop->second.mLongitude))
                ) * 6371000;
                geographicDistance += nextGeographicPiece;
                auto it = mStopDistances.find({ firstStop->first, secondStop->first });
                busInfo.mDistance += it == mStopDistances.end() ? nextGeographicPiece : it->second;
                firstStop = secondStop;
            }
            std::sort(busInfo.mStops.begin(), busInfo.mStops.end());
            busInfo.mUniqueStops = std::unique(busInfo.mStops.begin(), busInfo.mStops.end()) - busInfo.mStops.begin();
            busInfo.mCurvature = busInfo.mDistance / geographicDistance;
        }
    }

    void printBusInfo(const std::string& name, std::ostream& out) {
        auto it = mBuses.find(name);
        if (it != mBuses.end()) {
            const auto& busInfo = it->second;
            out << "Bus " << name << ": " << busInfo.mStops.size() << " stops on route, " << busInfo.mUniqueStops << " unique stops, "
                << busInfo.mDistance << " route length, " << busInfo.mCurvature << " curvature\n";
        }
        else {
            out << "Bus " << name << ": not found\n";
        }
    }

    void printStopInfo(const std::string& name, std::ostream& out) {
        auto it = mStops.find(name);
        if (it != mStops.end()) {
            const auto& stopInfo = it->second;
            out << "Stop " << name << ": ";
            if (stopInfo.mBuses.size() == 0) {
                out << "no buses";
            }
            else {
                out << "buses";
                for (const auto& bus : stopInfo.mBuses) {
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
    using Stops = std::map<std::string, StopInfo>;
    using Buses = std::map<std::string, BusInfo>;
    
    Stops mStops;
    Buses mBuses;
    std::map<std::pair<std::string, std::string>, double> mStopDistances;
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
            iss.ignore();
            std::map<std::string, double> stopDistances;
            double distance;
            while (iss >> distance) {
                iss.ignore(5);
                std::string stop;
                std::getline(iss, stop, ',');
                stopDistances.emplace(std::move(stop), distance);
            }
            guide.addStop(std::move(stopName), std::move(stopDistances), latitude, longitude);
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