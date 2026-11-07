#pragma once

#include <string>
#include <vector>

namespace erl {

class ReliefVehicle;

struct RouteEvaluation {
    bool reachable{false};
    bool damaged{false};
    std::string message;
};

class RouteGraph {
public:
    RouteGraph();

    RouteEvaluation evaluate(const std::string& corridor,
                             const ReliefVehicle& vehicle) const;

private:
    struct Edge {
        std::string from;
        std::string to;
        std::string corridor;
        bool damaged{false};
    };

    std::vector<Edge> edges_;
};

} // namespace erl
