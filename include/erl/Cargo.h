#pragma once

#include <iosfwd>
#include <string>

namespace erl {

class ReliefCargo {
public:
    ReliefCargo() = default;
    ReliefCargo(std::string id, std::string category, int quantity,
                double unitWeightKg, int priority = 3);
    ~ReliefCargo() = default;

    const std::string& id() const noexcept { return id_; }
    const std::string& category() const noexcept { return category_; }
    int quantity() const noexcept { return quantity_; }
    double unitWeightKg() const noexcept { return unitWeightKg_; }
    int priority() const noexcept { return priority_; }

    ReliefCargo& operator+=(int quantity);
    ReliefCargo& operator-=(int quantity);
    bool operator==(const ReliefCargo& other) const noexcept;

private:
    std::string id_;
    std::string category_;
    int quantity_{0};
    double unitWeightKg_{0.0};
    int priority_{3};
};

std::ostream& operator<<(std::ostream&, const ReliefCargo&);
std::istream& operator>>(std::istream&, ReliefCargo&);

} // namespace erl

