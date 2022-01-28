#pragma once

#include <set>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <iostream>

#include "router.h"

struct StopInfo {
    std::set<std::string> mBuses;
    double mLatitude;
    double mLongitude;
    size_t mId;

    static inline size_t IdCounter = 0;
};

struct BusInfo {
    std::vector<std::string> mStops;

    size_t mUniqueStops;
    double mDistance;
    double mCurvature;
};

class TransportGuide {
public:
    TransportGuide()
    {
    }

    void addStop(const std::string& name, const std::vector<std::pair<std::string, double>>& stopDistances, double latitude, double longitude) {
        for (auto& stop : stopDistances) {
            mStopDistances[{name, stop.first}] = stop.second;
            if (mStopDistances.find({ stop.first, name }) == mStopDistances.end()) {
                mStopDistances[{ stop.first, name }] = stop.second;
            }
        }
        mStops.emplace(name, StopInfo{ {}, latitude, longitude, StopInfo::IdCounter++ });
        mStopMapIdName.emplace_back(name);
    }

    void addBus(const std::string& name, std::vector<std::string>&& stops, bool isRoundTrip) {
        if (!isRoundTrip) {
            stops.reserve(stops.size() * 2);
            std::copy(std::next(stops.rbegin()), stops.rend(), std::back_inserter(stops));
        }
        mBuses.emplace(name, BusInfo{ std::move(stops), 0, 0.0, 0.0 });
    }

    void init() {
        Graph::DirectedWeightedGraph<double> graph(mStops.size() * 2);
        for (size_t i = 0; i < mStops.size(); ++i) {
            graph.AddEdge({ i, mStops.size() + i, mRoutingSettings.mBusWaitTime });
        }

        for (auto& [busName, busInfo] : mBuses) {
            Stops::iterator firstStop = mStops.find(busInfo.mStops[0]);
            firstStop->second.mBuses.insert(busName);
            Stops::iterator secondStop;
            double geographicDistance = 0.0;

            std::vector<double> stopTimes(busInfo.mStops.size(), 0.0);

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
                double distancePiece = it == mStopDistances.end() ? nextGeographicPiece : it->second;
                busInfo.mDistance += distancePiece;

                stopTimes[i] = busInfo.mDistance / mRoutingSettings.mBusVelocity;

                firstStop = secondStop;
            }

            for (size_t i = 0; i < busInfo.mStops.size(); ++i)
            {
                size_t start = mStops.find(busInfo.mStops[i])->second.mId;
                for (size_t j = i+1; j < busInfo.mStops.size(); ++j)
                {
                    size_t finish = mStops.find(busInfo.mStops[j])->second.mId;
                    if (busInfo.mStops[i] != busInfo.mStops[j]) {
                        auto edgeId = graph.AddEdge({ mStops.size() + start, finish, (stopTimes[j] - stopTimes[i]) / 60.0 });
                        mEdges[edgeId] = { busName, j - i };
                    }
                }
            }

            auto stopsCopy = busInfo.mStops;
            std::sort(stopsCopy.begin(), stopsCopy.end());
            busInfo.mUniqueStops = std::unique(stopsCopy.begin(), stopsCopy.end()) - stopsCopy.begin();
            busInfo.mCurvature = busInfo.mDistance / geographicDistance;
        }
        mRouter.SetGraph(std::move(graph));
    }

    void setRoutingSettings(double busWaitTime, double busVelocity) {
        busVelocity /= 3.6;
        mRoutingSettings = { busWaitTime, busVelocity };
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

    void printRouteInfo(const std::string& from, const std::string& to, long id, std::ostream& out) const {
        auto fromIt = mStops.find(from);
        auto toIt = mStops.find(to);
        std::vector<size_t> fromVertexes;
        fromVertexes.reserve(fromIt->second.mBuses.size());
        std::vector<size_t> toVertexes;
        toVertexes.reserve(toIt->second.mBuses.size());

        auto minInfo = mRouter.BuildRoute(mStops.find(from)->second.mId, mStops.find(to)->second.mId);

        out << "\"request_id\": " << id << ",\n";
        if (!minInfo) {
            out << "\"error_message\": \"not found\"\n";
        }
        else {
            double totalTime = 0.0;
            out << "\"items\": [\n";
            const auto& graph = mRouter.GetGraph();
            for (size_t i = 0; i < minInfo->edge_count; ++i) {
                size_t edgeId = mRouter.GetRouteEdge(minInfo->id, i);
                const auto& edge = graph.GetEdge(edgeId);
                if (edge.to - edge.from == mStops.size()) {
                    out << "{\"type\": \"Wait\",\"stop_name\": \"" << mStopMapIdName[edge.from] << "\",\"time\": " << mRoutingSettings.mBusWaitTime << "}";
                    totalTime += mRoutingSettings.mBusWaitTime;
                }
                else {
                    const auto& edgeInfo = mEdges.at(edgeId);
                    out << "{\"type\": \"Bus\",\"bus\": \"" << edgeInfo.mBusName << "\",\"span_count\": " << edgeInfo.mSpanCount << ",\"time\": " << edge.weight << "}";
                    totalTime += edge.weight;
                }
                if (i != minInfo->edge_count - 1) {
                    out << ",";
                }
            }
            out << "],\n";
            out << "\"total_time\":" << totalTime << "\n";
        }
    }
private:
    struct RoutingSettings {
        double mBusWaitTime = 0.0;
        double mBusVelocity = 0.0;
    };

    struct EdgeInfo {
        std::string mBusName;
        size_t mSpanCount = 0;
    };

    using Stops = std::map<std::string, StopInfo>;
    using Buses = std::map<std::string, BusInfo>;

    Stops mStops;

    std::vector<std::string> mStopMapIdName;

    Buses mBuses;
    std::map<std::pair<std::string, std::string>, double> mStopDistances;

    std::map<size_t, EdgeInfo> mEdges;

    RoutingSettings mRoutingSettings;

    Graph::Router<double> mRouter;
};