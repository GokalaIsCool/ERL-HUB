#include "erl/System.h"

#include "erl/Formatters.h"
#include "erl/PriorityDispatchQueue.h"
#include "erl/Utilities.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace erl {

bool ReliefHub::addDepot(const DistributionDepot& depotValue) {
    return depots_.emplace(depotValue.id(), depotValue).second;
}

bool ReliefHub::addVehicle(std::unique_ptr<ReliefVehicle> vehicleValue) {
    if (!vehicleValue) return false;
    for (const auto& [existingId, existingVehicle] : vehicles_) {
        if (*existingVehicle == *vehicleValue) return false;
    }
    return vehicles_.emplace(vehicleValue->id(), std::move(vehicleValue)).second;
}

bool ReliefHub::addMission(const ReliefMission& missionValue) {
    if (mission(missionValue.id()) != nullptr) return false;
    missions_.push_back(missionValue);
    return true;
}

DistributionDepot* ReliefHub::depot(const std::string& id) {
    const auto found = depots_.find(id);
    return found == depots_.end() ? nullptr : &found->second;
}

const DistributionDepot* ReliefHub::depot(const std::string& id) const {
    const auto found = depots_.find(id);
    return found == depots_.end() ? nullptr : &found->second;
}

ReliefVehicle* ReliefHub::vehicle(const std::string& id) {
    const auto found = vehicles_.find(id);
    return found == vehicles_.end() ? nullptr : found->second.get();
}

const ReliefVehicle* ReliefHub::vehicle(const std::string& id) const {
    const auto found = vehicles_.find(id);
    return found == vehicles_.end() ? nullptr : found->second.get();
}

ReliefMission* ReliefHub::mission(const std::string& id) {
    for (auto& item : missions_) if (item.id() == id) return &item;
    return nullptr;
}

const ReliefMission* ReliefHub::mission(const std::string& id) const {
    for (const auto& item : missions_) if (item.id() == id) return &item;
    return nullptr;
}

DispatchResult ReliefHub::dispatchMission(const std::string& missionId,
                                          const std::string& vehicleId) {
    auto* missionValue = mission(missionId);
    auto* vehicleValue = vehicle(vehicleId);
    if (!missionValue || !vehicleValue) return {false, "Mission or vehicle not found"};
    auto* depotValue = depot(missionValue->originDepot());
    if (!depotValue) return {false, "Mission origin depot not found"};

    const auto result = DispatchCoordinator::dispatch(*missionValue, *depotValue, *vehicleValue);
    if (result.success) history_.push_back(missionId + ":" + vehicleId + ":" + result.message);
    return result;
}

std::string ReliefHub::runtimeDispatchDemo() const {
    std::ostringstream report;
    report << "RUNTIME POLYMORPHISM DEMONSTRATION\n";
    for (const auto& [id, ownedVehicle] : vehicles_) {
        const ReliefVehicle* basePointer = ownedVehicle.get();
        report << id << " -> kind=" << basePointer->kind()
               << ", range=" << basePointer->rangeKm() << " km"
               << ", road=" << (basePointer->supportsCorridor("road") ? "yes" : "no")
               << ", airborne=" << (basePointer->isAirborne() ? "yes" : "no")
               << ", life-support=" << (basePointer->lifeSupport().active() ? "active" : "inactive")
               << ", beacon=(" << basePointer->beacon().coordinate().latitude << ','
               << basePointer->beacon().coordinate().longitude << ")\n";
    }
    return report.str();
}

std::string ReliefHub::priorityDispatchReport() const {
    std::vector<ReliefMission> sorted = missions_;
    std::stable_sort(sorted.begin(), sorted.end(), [](const ReliefMission& left, const ReliefMission& right) {
        return UrgentMedicalFirst{}(right, left);
    });

    PriorityDispatchQueue<ReliefMission, UrgentMedicalFirst> queue;
    for (const auto& item : missions_) queue.push(item);

    std::ostringstream report;
    report << "PRIORITY DISPATCH ORDER\nSorted: ";
    for (const auto& item : sorted) report << item.id() << ' ';
    report << "\nQueue: ";
    while (!queue.empty()) report << queue.pop().id() << ' ';
    report << '\n';
    return report.str();
}

std::string ReliefHub::auditReport() const {
    std::ostringstream report;
    report << "ERL-HUB DISPATCH AUDIT\n";
    report << runtimeDispatchDemo();
    report << priorityDispatchReport();
    report << "Registered vehicles: " << TransportVehicle::totalRegistered() << '\n';
    report << "Network depot capacity: " << DistributionDepot::totalCapacityKg() << " kg\n";
    report << tableSeparator;
    report << tableRow << std::left << std::setw(16) << "Mission ID"
           << tableRow << std::setw(16) << "Type"
           << tableRow << std::setw(16) << "Status"
           << tableRow << std::setw(16) << "Vehicle" << "|\n";
    report << tableSeparator;
    for (const auto& item : missions_) {
        report << tableRow << std::setw(16) << item.id()
               << tableRow << std::setw(16) << toString(item.type())
               << tableRow << std::setw(16) << toString(item.status())
               << tableRow << std::setw(16)
               << (item.assignedVehicleId().empty() ? "-" : item.assignedVehicleId()) << "|\n";
    }
    report << tableSeparator;
    for (const auto& [id, depotValue] : depots_) {
        std::ostringstream serializedDepot;
        serializedDepot << depotValue;
        report << "Depot record " << id << ": " << serializedDepot.str() << '\n';
        for (const auto& cargoValue : depotValue.inventory()) {
            std::ostringstream serializedCargo;
            serializedCargo << cargoValue;
            report << "  Cargo: " << serializedCargo.str() << '\n';
        }
    }
    for (const auto& [id, vehicleValue] : vehicles_) {
        std::ostringstream serializedVehicle;
        serializedVehicle << static_cast<const TransportVehicle&>(*vehicleValue);
        report << "Vehicle record " << id << ": " << serializedVehicle.str() << '\n';
    }
    report << "History entries: " << history_.size() << '\n';
    return report.str();
}

void ReliefHub::loadDemoScenario() {
    clear();

    DistributionDepot depotValue;
    std::istringstream depotInput("D-01 CentralDepot 12000 0");
    depotInput >> depotValue;

    ReliefCargo medical;
    ReliefCargo water;
    ReliefCargo shelter;
    std::istringstream cargoInput("MED-KIT medical 100 5 1\nWATER water 500 1 2\nTENT shelter 50 20 2");
    cargoInput >> medical >> water >> shelter;
    ((depotValue += medical) += water) += shelter;
    addDepot(depotValue);

    addVehicle(makeVehicle("Ground", "V-101", "MAIT Response", 2500, 600, {28.61, 77.23}));
    addVehicle(makeVehicle("Aerial", "V-202", "Air Relief", 800, 400, {28.61, 77.23}));
    addVehicle(makeVehicle("Amphibious", "V-303", "River Relief", 1500, 300, {28.61, 77.23}));

    const double wardDistance = ReliefOps::Logistics::distanceKm(28.61, 77.23, 28.95, 77.10);
    addMission(ReliefMission("M-1001", MissionType::Medical, "D-01", "Ward7", "MED-KIT", 20, wardDistance, "road", 1));
    addMission(ReliefMission("M-1002", MissionType::Shelter, "D-01", "Camp4", "TENT", 10, 35, "road", 2));
    addMission(ReliefMission("M-1003", MissionType::Water, "D-01", "FloodZone", "WATER", 100, 20, "flooded", 1));
    if (auto* criticalMission = mission("M-1001")) IncidentCommander::overrideCriticalTriage(*criticalMission, 9);
}

void ReliefHub::clear() {
    depots_.clear();
    vehicles_.clear();
    missions_.clear();
    history_.clear();
}

bool ReliefHub::save(const std::string& path, std::string& error) const { return BinaryPersistence::save(*this, path, error); }
bool ReliefHub::load(const std::string& path, std::string& error) { return BinaryPersistence::load(*this, path, error); }

} // namespace erl
