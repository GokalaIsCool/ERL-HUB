#pragma once

#include "erl/Mission.h"
#include "erl/Vehicle.h"

#include <string>

namespace erl {

struct DispatchResult {
    bool success{false};
    std::string message;
};

class DispatchCoordinator {
public:
    static DispatchResult evaluate(const ReliefMission&, const ReliefVehicle&);
    static DispatchResult dispatch(ReliefMission&, ReliefVehicle&);
};

} // namespace erl

