#pragma once
#include "erl/Depot.h"
#include "erl/Mission.h"
#include "erl/Route.h"
#include "erl/Vehicle.h"
#include <string>
namespace erl { struct DispatchResult { bool success{false}; std::string message; }; class DispatchCoordinator { public: static DispatchResult dispatch(ReliefMission&, DistributionDepot&, ReliefVehicle&); }; }

