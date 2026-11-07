#pragma once
#include "erl/Cargo.h"
#include "erl/Exceptions.h"
#include "erl/Types.h"
#include <iosfwd>
#include <memory>
#include <string>
namespace erl {
class TransportVehicle {
public:
    TransportVehicle(); TransportVehicle(std::string, std::string, double, double); TransportVehicle(const TransportVehicle&); TransportVehicle& operator=(const TransportVehicle&); virtual ~TransportVehicle();
    const std::string& id() const noexcept { return id_; } const std::string& agency() const noexcept { return agency_; } double capacityKg() const noexcept { return capacityKg_; } double fuelLitres() const noexcept { return fuelLitres_; } double manifestWeightKg() const noexcept { return manifestWeightKg_; } VehicleStatus status() const noexcept { return status_; }
    void setStatus(VehicleStatus s) noexcept { status_ = s; } void setFuelLitres(double f); bool canCarry(double w) const noexcept { return manifestWeightKg_ + w <= capacityKg_; } static int totalRegistered() noexcept { return totalRegistered_; }
    virtual std::string kind() const { return "Transport"; } virtual double rangeKm() const noexcept; virtual double fuelRequired(double) const noexcept;
    TransportVehicle& operator+=(const ReliefCargo&); TransportVehicle& operator-=(const ReliefCargo&); bool operator==(const TransportVehicle& other) const noexcept { return id_ == other.id_; }
    friend std::ostream& operator<<(std::ostream&, const TransportVehicle&); friend std::istream& operator>>(std::istream&, TransportVehicle&);
private:
    void clearManifest() noexcept; void swap(TransportVehicle& other) noexcept; std::string id_; std::string agency_; double capacityKg_{0.0}; double fuelLitres_{0.0}; double manifestWeightKg_{0.0}; VehicleStatus status_{VehicleStatus::Available}; ReliefCargo** manifest_{nullptr}; std::size_t manifestSize_{0}; std::size_t manifestCapacity_{0}; static int totalRegistered_;
};
class GPSBeacon { public: explicit GPSBeacon(GPSCoordinate c = {}) : coordinate_(c) {} GPSCoordinate coordinate() const noexcept { return coordinate_; } void setCoordinate(GPSCoordinate c) noexcept { coordinate_ = c; } private: GPSCoordinate coordinate_; };
class EmergencyLifeSupportUnit { public: explicit EmergencyLifeSupportUnit(bool active = true) : active_(active) {} bool active() const noexcept { return active_; } void setActive(bool a) noexcept { active_ = a; } private: bool active_{true}; };
class ReliefVehicle : public TransportVehicle { public: ReliefVehicle(std::string, std::string, double, double, GPSCoordinate = {}); ~ReliefVehicle() override; virtual bool supportsCorridor(const std::string&) const = 0; virtual bool isAirborne() const noexcept = 0; const GPSBeacon& beacon() const noexcept { return beacon_; } GPSBeacon& beacon() noexcept { return beacon_; } const EmergencyLifeSupportUnit& lifeSupport() const noexcept { return lifeSupport_; } virtual std::string kind() const override = 0; private: GPSBeacon beacon_; EmergencyLifeSupportUnit lifeSupport_; };
class GroundConvoy : public ReliefVehicle { public: GroundConvoy(std::string, std::string, double, double, GPSCoordinate = {}); std::string kind() const override { return "Ground"; } double rangeKm() const noexcept override; double fuelRequired(double) const noexcept override; bool supportsCorridor(const std::string&) const override; bool isAirborne() const noexcept override { return false; } };
class AerialTransport : public ReliefVehicle { public: AerialTransport(std::string, std::string, double, double, GPSCoordinate = {}); std::string kind() const override { return "Aerial"; } double rangeKm() const noexcept override; double fuelRequired(double) const noexcept override; bool supportsCorridor(const std::string&) const override { return true; } bool isAirborne() const noexcept override { return true; } };
class FuelPoweredVehicle : public virtual ReliefVehicle { public: FuelPoweredVehicle() : ReliefVehicle("", "", 0.0, 0.0) {} bool hasFuelFor(double d) const noexcept { return fuelLitres() >= fuelRequired(d); } };
class FloatCapableVehicle : public virtual ReliefVehicle { public: FloatCapableVehicle() : ReliefVehicle("", "", 0.0, 0.0) {} bool supportsWaterCorridor(const std::string& c) const { return c == "river" || c == "flooded"; } };
class AmphibiousCraft : public FuelPoweredVehicle, public FloatCapableVehicle { public: AmphibiousCraft(std::string, std::string, double, double, GPSCoordinate = {}); std::string kind() const override { return "Amphibious"; } double rangeKm() const noexcept override; double fuelRequired(double) const noexcept override; bool supportsCorridor(const std::string&) const override; bool isAirborne() const noexcept override { return false; } };
std::unique_ptr<ReliefVehicle> makeVehicle(const std::string&, const std::string&, const std::string&, double, double, GPSCoordinate = {});
} // namespace erl

