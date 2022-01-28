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
#include <variant>

#include <fstream>

namespace Json {

    class Node : std::variant<std::vector<Node>,
        std::map<std::string, Node>,
        double,
        std::string,
        bool> {
    public:
        using variant::variant;

        const auto& AsArray() const {
            return std::get<std::vector<Node>>(*this);
        }
        const auto& AsMap() const {
            return std::get<std::map<std::string, Node>>(*this);
        }
        double AsDouble() const {
            return std::get<double>(*this);
        }
        const auto& AsString() const {
            return std::get<std::string>(*this);
        }
        const auto& AsBoolean() const {
            return std::get<bool>(*this);
        }
    };

    class Document {
    public:
        explicit Document(Node&& root) : root(std::move(root)) { }

        const Node& GetRoot() const {
            return root;
        }

    private:
        Node root;
    };

    Document Load(std::istream& input);

    Node LoadNode(std::istream& input);

    Node LoadArray(std::istream& input) {
        std::vector<Node> result;

        for (char c; input >> c && c != ']'; ) {
            if (c != ',') {
                input.putback(c);
            }
            result.push_back(LoadNode(input));
        }

        return Node(move(result));
    }

    Node LoadOther(std::istream& input) {
        if (input.peek() == 't') {
            input.ignore(4);
            return Node(true);
        }
        else if (input.peek() == 'f') {
            input.ignore(5);
            return Node(false);
        }
        double result;
        std::cin >> result;
        return Node(result);
    }

    Node LoadString(std::istream& input) {
        std::string line;
        std::getline(input, line, '"');
        return Node(move(line));
    }

    Node LoadDict(std::istream& input) {
        std::map<std::string, Node> result;

        for (char c; input >> c && c != '}'; ) {
            if (c == ',') {
                input >> c;
            }

            std::string key = LoadString(input).AsString();
            input >> c;
            result.emplace(std::move(key), LoadNode(input));
        }

        return Node(move(result));
    }

    Node LoadNode(std::istream& input) {
        char c;
        input >> c;

        if (c == '[') {
            return LoadArray(input);
        }
        else if (c == '{') {
            return LoadDict(input);
        }
        else if (c == '"') {
            return LoadString(input);
        }
        else {
            input.putback(c);
            return LoadOther(input);
        }
    }

    Document Load(std::istream& input) {
        return Document{ LoadNode(input) };
    }
}

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
    void addStop(const std::string& name, const std::vector<std::pair<std::string, double>>& stopDistances, double latitude, double longitude) {
        for (auto& stop : stopDistances) {
            mStopDistances[{name, stop.first}] = stop.second;
            if (mStopDistances.find({ stop.first, name }) == mStopDistances.end()) {
                mStopDistances[{ stop.first, name }] = stop.second;
            }
        }
        mStops.emplace(name, StopInfo{ {}, latitude, longitude });
    }

    void addBus(const std::string& name, std::vector<std::string>&& stops, bool isRoundTrip) {
        if (!isRoundTrip) {
            stops.reserve(stops.size() * 2);
            std::copy(std::next(stops.rbegin()), stops.rend(), std::back_inserter(stops));
        }
        mBuses.emplace(name, BusInfo{ std::move(stops), 0, 0.0, 0.0 });
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

    void printBusInfo(const std::string& name, long id, std::ostream& out) const {
        auto it = mBuses.find(name);
        out << "\"request_id\": " << id << ",\n";
        if (it != mBuses.end()) {
            const auto& busInfo = it->second;
            out << "\"route_length\": " << busInfo.mDistance << ",\n";
            out << "\"curvature\": " << busInfo.mCurvature << ",\n";
            out << "\"stop_count\": " << busInfo.mStops.size() << ",\n";
            out << "\"unique_stop_count\": " << busInfo.mUniqueStops << "\n";
        }
        else {
            out << "\"error_message\": \"not found\"\n";
        }
    }

    void printStopInfo(const std::string& name, long id, std::ostream& out) const {
        auto it = mStops.find(name);
        out << "\"request_id\": " << id << ",\n";
        if (it != mStops.end()) {
            const auto& stopInfo = it->second;
            if (stopInfo.mBuses.size() == 0) {
                out << "\"buses\": []\n";
            }
            else {
                out << "\"buses\": [";
                for (auto it = stopInfo.mBuses.begin(); it != std::prev(stopInfo.mBuses.end()); ++it) {
                    out << "\n\"" << *it << "\",";
                }
                out << "\n\"" << *stopInfo.mBuses.rbegin() << "\"";
                out << "\n]\n";
            }
        }
        else {
            out << "\"error_message\": \"not found\"\n";
        }
    }
private:
    using Stops = std::map<std::string, StopInfo>;
    using Buses = std::map<std::string, BusInfo>;
    
    Stops mStops;
    Buses mBuses;
    std::map<std::pair<std::string, std::string>, double> mStopDistances;
};

void processStatRequest(const TransportGuide& guide, const Json::Node& request) {
    const auto& requestDict = request.AsMap();
    std::cout << "\n{\n";
    if (requestDict.at("type").AsString() == "Stop") {
        guide.printStopInfo(requestDict.at("name").AsString(), requestDict.at("id").AsDouble(), std::cout);
    }
    else {
        guide.printBusInfo(requestDict.at("name").AsString(), requestDict.at("id").AsDouble(), std::cout);
    }
    std::cout << "}";
}

int main() {
    std::cout.precision(6);

    //std::ifstream in("a.txt");
    //std::streambuf* cinbuf = std::cin.rdbuf();
    //std::cin.rdbuf(in.rdbuf());

    auto document = Json::Load(std::cin);
    TransportGuide guide;

    for (const auto& request : document.GetRoot().AsMap().at("base_requests").AsArray()) {
        const auto& requestDict = request.AsMap();
        if (requestDict.at("type").AsString() == "Stop") {
            auto& stopDistanceNodes = requestDict.at("road_distances").AsMap();
            std::vector<std::pair<std::string, double>> stopDistances;
            stopDistances.reserve(stopDistanceNodes.size());
            for (auto& [key, value] : stopDistanceNodes) {
                stopDistances.emplace_back(key, value.AsDouble());
            }
            guide.addStop(requestDict.at("name").AsString(),
                          stopDistances, 
                          requestDict.at("latitude").AsDouble() * 3.1415926535 / 180,
                          requestDict.at("longitude").AsDouble() * 3.1415926535 / 180
            );
        }
        else {
            auto& stopNodes = requestDict.at("stops").AsArray();
            std::vector<std::string> stops;
            stops.reserve(stopNodes.size());
            for (auto& node : stopNodes) {
                stops.push_back(node.AsString());
            }
            guide.addBus(requestDict.at("name").AsString(), std::move(stops), requestDict.at("is_roundtrip").AsBoolean());
        }
    }

    guide.init();

    const auto& statRequests = document.GetRoot().AsMap().at("stat_requests").AsArray();
    if (statRequests.size() > 0) {
        std::cout << "[";
        for (auto it = statRequests.begin(); it != std::prev(statRequests.end()); ++it) {
            processStatRequest(guide, *it);
            std::cout << ",";
        }
        processStatRequest(guide, statRequests.back());
        std::cout << "\n]";
    }
    return 0;
}