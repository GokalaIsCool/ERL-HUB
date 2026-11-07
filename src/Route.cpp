#include "erl/Route.h"

#include "erl/Vehicle.h"

#include <algorithm>
#include <queue>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace erl {

RouteGraph::RouteGraph()
    : edges_{{"DEPOT", "ROAD_CHECKPOINT", "road", false},
             {"ROAD_CHECKPOINT", "DESTINATION", "road", false},
             {"DEPOT", "HIGHWAY_CHECKPOINT", "highway", false},
             {"HIGHWAY_CHECKPOINT", "DESTINATION", "highway", false},
             {"DEPOT", "URBAN_CHECKPOINT", "urban", false},
             {"URBAN_CHECKPOINT", "DESTINATION", "urban", false},
             {"DEPOT", "RIVER_CHECKPOINT", "river", false},
             {"RIVER_CHECKPOINT", "DESTINATION", "river", false},
             {"DEPOT", "FLOOD_CHECKPOINT", "flooded", false},
             {"FLOOD_CHECKPOINT", "DESTINATION", "flooded", false},
             {"DEPOT", "DAMAGED_CHECKPOINT", "damaged", true},
             {"DAMAGED_CHECKPOINT", "DESTINATION", "damaged", true}} {}

RouteEvaluation RouteGraph::evaluate(const std::string& corridor,
                                     const ReliefVehicle& vehicle) const {
    const auto matchesCorridor = [&corridor](const Edge& edge) {
        return edge.corridor == corridor;
    };
    const auto matching = std::find_if(edges_.begin(), edges_.end(), matchesCorridor);
    if (matching == edges_.end()) {
        return {false, false, "No route is defined for corridor: " + corridor};
    }

    if (matching->damaged) {
        return {false, true, "Damaged corridor: " + corridor};
    }
    if (!vehicle.supportsCorridor(corridor)) {
        return {false, false, "Vehicle cannot use the requested corridor"};
    }

    std::queue<std::string> frontier;
    std::unordered_set<std::string> visited;
    std::unordered_map<std::string, std::string> previous;
    frontier.push("DEPOT");
    visited.insert("DEPOT");

    while (!frontier.empty()) {
        const std::string current = frontier.front();
        frontier.pop();
        if (current == "DESTINATION") break;

        for (const auto& edge : edges_) {
            if (edge.corridor != corridor || edge.damaged || edge.from != current ||
                visited.find(edge.to) != visited.end()) {
                continue;
            }
            visited.insert(edge.to);
            previous.emplace(edge.to, current);
            frontier.push(edge.to);
        }
    }

    if (visited.find("DESTINATION") == visited.end()) {
        return {false, false, "No safe route is available for corridor: " + corridor};
    }

    std::vector<std::string> path;
    for (std::string node = "DESTINATION";; node = previous.at(node)) {
        path.push_back(node);
        if (node == "DEPOT") break;
    }
    std::reverse(path.begin(), path.end());

    std::ostringstream message;
    message << "Safe route via ";
    for (std::size_t index = 0; index < path.size(); ++index) {
        if (index != 0) message << " -> ";
        message << path[index];
    }
    return {true, false, message.str()};
}

} // namespace erl
