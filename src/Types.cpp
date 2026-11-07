#include "erl/Types.h"
namespace erl {
std::string toString(MissionType t) { switch(t){case MissionType::Medical:return "Medical";case MissionType::Shelter:return "Shelter";case MissionType::Rescue:return "Rescue";case MissionType::Food:return "Food";case MissionType::Water:return "Water";} return "Food"; }
std::string toString(MissionStatus s) { switch(s){case MissionStatus::Planned:return "Planned";case MissionStatus::Assigned:return "Assigned";case MissionStatus::Dispatched:return "Dispatched";case MissionStatus::Completed:return "Completed";case MissionStatus::Failed:return "Failed";} return "Planned"; }
std::string toString(VehicleStatus s) { switch(s){case VehicleStatus::Available:return "Available";case VehicleStatus::Assigned:return "Assigned";case VehicleStatus::InTransit:return "InTransit";case VehicleStatus::Maintenance:return "Maintenance";} return "Available"; }
MissionType missionTypeFromString(const std::string& v) { if(v=="Medical")return MissionType::Medical;if(v=="Shelter")return MissionType::Shelter;if(v=="Rescue")return MissionType::Rescue;if(v=="Water")return MissionType::Water;return MissionType::Food; }
}

