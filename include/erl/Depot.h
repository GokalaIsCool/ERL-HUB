#pragma once
#include "erl/Cargo.h"
#include <iosfwd>
#include <string>
#include <unordered_map>
#include <vector>
namespace erl {
class DistributionDepot {
public:
    DistributionDepot() = default; DistributionDepot(std::string id, std::string name, double capacityKg);
    DistributionDepot(const DistributionDepot&); DistributionDepot(DistributionDepot&&) noexcept; DistributionDepot& operator=(const DistributionDepot&); DistributionDepot& operator=(DistributionDepot&&) noexcept; ~DistributionDepot();
    const std::string& id() const noexcept { return id_; } const std::string& name() const noexcept { return name_; } double capacityKg() const noexcept { return capacityKg_; } double currentWeightKg() const noexcept { return currentWeightKg_; } double remainingCapacityKg() const noexcept { return capacityKg_ - currentWeightKg_; } static double totalCapacityKg() noexcept { return totalCapacityKg_; }
    bool addCargo(const ReliefCargo&); bool removeCargo(const std::string&, int); ReliefCargo* cargo(const std::string&); const ReliefCargo* cargo(const std::string&) const; std::vector<ReliefCargo> inventory() const;
    DistributionDepot& operator+=(const ReliefCargo&); DistributionDepot& operator-=(const ReliefCargo&);
    friend std::ostream& operator<<(std::ostream&, const DistributionDepot&); friend std::istream& operator>>(std::istream&, DistributionDepot&);
private: std::string id_; std::string name_; double capacityKg_{0.0}; double currentWeightKg_{0.0}; std::unordered_map<std::string, ReliefCargo> inventory_; static double totalCapacityKg_;
};
} // namespace erl
namespace ReliefOps::Inventory { bool allocateSupply(erl::DistributionDepot&, const std::string&, int, erl::ReliefCargo&); bool allocateSupply(erl::DistributionDepot&, erl::ReliefCargo&, int, erl::ReliefCargo&); }

