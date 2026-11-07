#pragma once

#include "erl/Cargo.h"

#include <iosfwd>
#include <string>
#include <unordered_map>

namespace erl {

class DistributionDepot {
public:
    DistributionDepot() = default;
    DistributionDepot(std::string id, std::string name, double capacityKg);
    ~DistributionDepot() = default;

    const std::string& id() const noexcept { return id_; }
    const std::string& name() const noexcept { return name_; }
    double capacityKg() const noexcept { return capacityKg_; }

    bool addCargo(const ReliefCargo& cargo);
    bool removeCargo(const std::string& cargoId, int quantity);
    ReliefCargo* cargo(const std::string& cargoId);

    DistributionDepot& operator+=(const ReliefCargo& cargo);
    DistributionDepot& operator-=(const ReliefCargo& cargo);

private:
    std::string id_;
    std::string name_;
    double capacityKg_{0.0};
    std::unordered_map<std::string, ReliefCargo> inventory_;
};

std::ostream& operator<<(std::ostream&, const DistributionDepot&);
std::istream& operator>>(std::istream&, DistributionDepot&);

} // namespace erl

namespace ReliefOps::Inventory {

bool allocateSupply(erl::DistributionDepot&, const std::string&, int, erl::ReliefCargo&);
bool allocateSupply(erl::DistributionDepot&, erl::ReliefCargo&, int, erl::ReliefCargo&);

} // namespace ReliefOps::Inventory

