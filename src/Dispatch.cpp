#include "erl/Dispatch.h"

#include "erl/Utilities.h"

#include <sstream>

namespace erl {

DispatchResult DispatchCoordinator::dispatch(ReliefMission& mission,
                                              DistributionDepot& depot,
                                              ReliefVehicle& vehicle) {
    try {
        if (mission.status() == MissionStatus::Completed || mission.status() == MissionStatus::Dispatched) {
            return {false, "Mission has already been dispatched"};
        }
        if (vehicle.status() != VehicleStatus::Available) {
            return {false, "Vehicle is not available"};
        }

        const ReliefCargo* stock = depot.cargo(mission.cargoId());
        if (stock == nullptr || stock->quantity() < mission.quantity()) {
            return {false, "Insufficient depot inventory"};
        }

        const double weight = mission.quantity() * stock->unitWeightKg();
        if (!vehicle.canCarry(weight)) {
            throw CargoOverweightException(weight + vehicle.manifestWeightKg(), vehicle.capacityKg());
        }
        if (mission.corridor() == "damaged") {
            throw DamagedCorridorException(mission.corridor());
        }
        if (!vehicle.supportsCorridor(mission.corridor())) {
            return {false, "Vehicle cannot use the requested corridor"};
        }

        const double requiredFuel = vehicle.fuelRequired(mission.distanceKm());
        if (vehicle.fuelLitres() < requiredFuel) {
            throw FuelDepletionException(requiredFuel, vehicle.fuelLitres());
        }

        ReliefCargo allocated;
        // Urgent medical missions use the ID overload; other missions use the
        // call-by-reference overload. Both paths commit through the same guard.
        if (mission.type() == MissionType::Medical) {
            if (!ReliefOps::Inventory::allocateSupply(depot, mission.cargoId(), mission.quantity(), allocated)) {
                return {false, "Inventory changed before dispatch could commit"};
            }
        } else {
            ReliefCargo sourceCopy = *stock;
            if (!ReliefOps::Inventory::allocateSupply(depot, sourceCopy, mission.quantity(), allocated)) {
                return {false, "Inventory changed before dispatch could commit"};
            }
        }

        try {
            vehicle += allocated;
            vehicle.setFuelLitres(vehicle.fuelLitres() - requiredFuel);
            vehicle.setStatus(VehicleStatus::InTransit);
            mission.assignVehicle(vehicle.id());
            mission.markDispatched();
            mission.markCompleted();
            vehicle -= allocated;
            vehicle.setStatus(VehicleStatus::Available);
        } catch (...) {
            depot += allocated;
            throw;
        }

        std::ostringstream message;
        message << "Dispatch completed; consumed " << allocated.quantity() << ' '
                << allocated.category() << " ("
                << ReliefOps::Logistics::payloadPercent(weight, vehicle.capacityKg())
                << "% payload) and " << requiredFuel << " litres of fuel";
        return {true, message.str()};
    } catch (const std::exception& exception) {
        return {false, exception.what()};
    }
}

} // namespace erl
