#pragma once

#include "erl/Cargo.h"
#include "erl/Types.h"

#include <iosfwd>
#include <memory>
#include <string>

namespace erl {

class TransportVehicle {
public:
    TransportVehicle();
    TransportVehicle(std::string id, std::string agency, double capacityKg, double fuelLitres);
    TransportVehicle(const TransportVehicle&);
    TransportVehicle& operator=(const TransportVehicle&);
    virtual ~TransportVehicle();

    virtual std::string kind() const;
    double capacityKg() const noexcept { return capacityKg_; }

private:
    std::string id_;
    std::string agency_;
    double capacityKg_{0.0};
    double fuelLitres_{0.0};
};

class GPSBeacon {
public:
    explicit GPSBeacon(GPSCoordinate coordinate = {}) : coordinate_(coordinate) {}
private:
    GPSCoordinate coordinate_;
};

class EmergencyLifeSupportUnit {
public:
    explicit EmergencyLifeSupportUnit(bool active = true) : active_(active) {}
private:
    bool active_{true};
};

class ReliefVehicle : public TransportVehicle {
public:
    ReliefVehicle(std::string id, std::string agency, double capacityKg, double fuelLitres);
    ~ReliefVehicle() override;

    virtual bool supportsCorridor(const std::string&) const = 0;
    virtual bool isAirborne() const noexcept = 0;
    virtual std::string kind() const override = 0;

private:
    GPSBeacon beacon_;
    EmergencyLifeSupportUnit lifeSupport_;
};

class GroundConvoy : public ReliefVehicle {
public:
    GroundConvoy(std::string id, std::string agency, double capacityKg, double fuelLitres);
    std::string kind() const override;
    bool supportsCorridor(const std::string&) const override;
    bool isAirborne() const noexcept override;
};

class AerialTransport : public ReliefVehicle {
public:
    AerialTransport(std::string id, std::string agency, double capacityKg, double fuelLitres);
    std::string kind() const override;
    bool supportsCorridor(const std::string&) const override;
    bool isAirborne() const noexcept override;
};

class FuelPoweredVehicle : public virtual ReliefVehicle {
public:
    FuelPoweredVehicle();
};

class FloatCapableVehicle : public virtual ReliefVehicle {
public:
    FloatCapableVehicle();
};

class AmphibiousCraft : public FuelPoweredVehicle, public FloatCapableVehicle {
public:
    AmphibiousCraft(std::string id, std::string agency, double capacityKg, double fuelLitres);
    std::string kind() const override;
    bool supportsCorridor(const std::string&) const override;
    bool isAirborne() const noexcept override;
};

std::unique_ptr<ReliefVehicle> makeVehicle(const std::string& kind,
                                            const std::string& id,
                                            const std::string& agency,
                                            double capacityKg,
                                            double fuelLitres);

} // namespace erl

