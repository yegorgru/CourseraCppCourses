#include <sstream>
#include <fstream>

#include "router.h"
#include "graph.h"
#include "transport_guide.h"
#include "json.h"

void processStatRequest(const TransportGuide& guide, const Json::Node& request) {
    const auto& requestDict = request.AsMap();
    std::cout << "\n{\n";
    if (requestDict.at("type").AsString() == "Stop") {
        guide.printStopInfo(requestDict.at("name").AsString(), requestDict.at("id").AsDouble(), std::cout);
    }
    else if (requestDict.at("type").AsString() == "Bus") {
        guide.printBusInfo(requestDict.at("name").AsString(), requestDict.at("id").AsDouble(), std::cout);
    }
    else {
        guide.printRouteInfo(requestDict.at("from").AsString(), requestDict.at("to").AsString(), requestDict.at("id").AsDouble(), std::cout);
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

    const auto& routingSettingsMap = document.GetRoot().AsMap().at("routing_settings").AsMap();
    guide.setRoutingSettings(routingSettingsMap.at("bus_wait_time").AsDouble(), routingSettingsMap.at("bus_velocity").AsDouble());

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