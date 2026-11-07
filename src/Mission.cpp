#include "erl/Mission.h"
#include <istream>
#include <ostream>
#include <stdexcept>
#include <utility>
namespace erl {
ReliefMission::ReliefMission(std::string id,MissionType type,std::string origin,std::string destination,std::string cargo,int quantity,double distance,std::string corridor,int priority):id_(std::move(id)),type_(type),originDepot_(std::move(origin)),destination_(std::move(destination)),cargoId_(std::move(cargo)),quantity_(quantity),distanceKm_(distance),corridor_(std::move(corridor)),priority_(priority){if(quantity<=0||distance<0)throw std::invalid_argument("Mission values are invalid");}
void ReliefMission::assignVehicle(const std::string&id){if(id.empty())throw std::invalid_argument("Vehicle ID cannot be empty");assignedVehicleId_=id;status_=MissionStatus::Assigned;} void ReliefMission::markDispatched(){status_=MissionStatus::Dispatched;} void ReliefMission::markCompleted(){status_=MissionStatus::Completed;} void ReliefMission::markFailed(){status_=MissionStatus::Failed;} int ReliefMission::triageScore()const noexcept{return priority_*10+(type_==MissionType::Medical?20:0)+(critical_?100:0);}
void IncidentCommander::overrideCriticalTriage(ReliefMission&m,int urgency){if(urgency>=8){m.critical_=true;m.priority_=1;}} bool UrgentMedicalFirst::operator()(const ReliefMission&a,const ReliefMission&b)const noexcept{return a.triageScore()!=b.triageScore()?a.triageScore()<b.triageScore():a.id()>b.id();}
std::ostream&operator<<(std::ostream&o,const ReliefMission&m){return o<<m.id_<<' '<<toString(m.type_)<<' '<<m.originDepot_<<' '<<m.destination_<<' '<<m.cargoId_<<' '<<m.quantity_<<' '<<m.distanceKm_<<' '<<m.corridor_<<' '<<m.priority_;} std::istream&operator>>(std::istream&i,ReliefMission&m){std::string t;i>>m.id_>>t>>m.originDepot_>>m.destination_>>m.cargoId_>>m.quantity_>>m.distanceKm_>>m.corridor_>>m.priority_;m.type_=missionTypeFromString(t);return i;}
}

