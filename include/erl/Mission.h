#pragma once

#include "erl/Types.h"

#include <iosfwd>
#include <string>

namespace erl {

class IncidentCommander;

class ReliefMission {
public:
    ReliefMission() = default;
    ReliefMission(std::string id, MissionType type, std::string originDepot,
                  std::string destination, std::string cargoId, int quantity,
                  double distanceKm, std::string corridor, int priority = 3);
    ~ReliefMission() = default;

    const std::string& id() const noexcept { return id_; }
    MissionStatus status() const noexcept { return status_; }
    void assignVehicle(const std::string& vehicleId);

private:
    friend class IncidentCommander;
    std::string id_;
    MissionType type_{MissionType::Food};
    std::string originDepot_;
    std::string destination_;
    std::string cargoId_;
    int quantity_{0};
    double distanceKm_{0.0};
    std::string corridor_;
    int priority_{3};
    MissionStatus status_{MissionStatus::Planned};
};

class IncidentCommander {
public:
    static void overrideCriticalTriage(ReliefMission&, int urgencyScore);
};

struct UrgentMedicalFirst {
    bool operator()(const ReliefMission&, const ReliefMission&) const noexcept;
};

} // namespace erl

