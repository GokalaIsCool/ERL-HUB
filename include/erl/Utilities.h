#pragma once
#include <cmath>
namespace ReliefOps::Logistics {
inline double distanceKm(double latitude1, double longitude1, double latitude2, double longitude2) {
    constexpr double pi = 3.14159265358979323846; constexpr double earthRadiusKm = 6371.0;
    const auto radians = [pi](double degrees) { return degrees * pi / 180.0; };
    const double dLat = radians(latitude2 - latitude1), dLon = radians(longitude2 - longitude1);
    const double lat1 = radians(latitude1), lat2 = radians(latitude2);
    const double a = std::sin(dLat / 2.0) * std::sin(dLat / 2.0) + std::cos(lat1) * std::cos(lat2) * std::sin(dLon / 2.0) * std::sin(dLon / 2.0);
    return earthRadiusKm * 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
}
inline double payloadPercent(double payloadKg, double capacityKg) { return capacityKg <= 0.0 ? 0.0 : payloadKg / capacityKg * 100.0; }
} // namespace ReliefOps::Logistics
namespace ReliefOps::Inventory { inline bool canAllocate(int available, int requested) { return requested > 0 && available >= requested; } }

