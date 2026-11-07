#pragma once
#include "erl/Dispatch.h"
#include "erl/Persistence.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
namespace erl { class ReliefHub { public: bool addDepot(const DistributionDepot&); bool addVehicle(std::unique_ptr<ReliefVehicle>); bool addMission(const ReliefMission&); DistributionDepot* depot(const std::string&); const DistributionDepot* depot(const std::string&) const; ReliefVehicle* vehicle(const std::string&); const ReliefVehicle* vehicle(const std::string&) const; ReliefMission* mission(const std::string&); const ReliefMission* mission(const std::string&) const; const std::unordered_map<std::string, DistributionDepot>& depots() const noexcept { return depots_; } const std::unordered_map<std::string, std::unique_ptr<ReliefVehicle>>& vehicles() const noexcept { return vehicles_; } const std::vector<ReliefMission>& missions() const noexcept { return missions_; } const std::vector<std::string>& history() const noexcept { return history_; } DispatchResult dispatchMission(const std::string&, const std::string&); std::string auditReport() const; void loadDemoScenario(); void clear(); bool save(const std::string&, std::string&) const; bool load(const std::string&, std::string&); private: friend class BinaryPersistence; std::unordered_map<std::string, DistributionDepot> depots_; std::unordered_map<std::string, std::unique_ptr<ReliefVehicle>> vehicles_; std::vector<ReliefMission> missions_; std::vector<std::string> history_; }; }

