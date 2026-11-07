#pragma once
#include <string>
namespace erl {
enum class MissionType { Medical, Shelter, Rescue, Food, Water };
enum class MissionStatus { Planned, Assigned, Dispatched, Completed, Failed };
enum class VehicleStatus { Available, Assigned, InTransit, Maintenance };
std::string toString(MissionType type);
std::string toString(MissionStatus status);
std::string toString(VehicleStatus status);
MissionType missionTypeFromString(const std::string& value);
struct GPSCoordinate { double latitude{0.0}; double longitude{0.0}; };
} // namespace erl

