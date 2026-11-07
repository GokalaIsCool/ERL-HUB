#pragma once

#include "erl/Depot.h"
#include "erl/Mission.h"
#include "erl/Vehicle.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace erl {

class ReliefHub {
public:
    ReliefHub() = default;
    ~ReliefHub() = default;

    bool addDepot(const DistributionDepot&);
    bool addVehicle(std::unique_ptr<ReliefVehicle>);
    bool addMission(const ReliefMission&);
    void printPhase1Status() const;

private:
    std::unordered_map<std::string, DistributionDepot> depots_;
    std::unordered_map<std::string, std::unique_ptr<ReliefVehicle>> vehicles_;
    std::vector<ReliefMission> missions_;
};

} // namespace erl

