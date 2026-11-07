#include "erl/Persistence.h"

#include "erl/System.h"

#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>

namespace erl {
namespace {

constexpr std::uint32_t kMagic = 0x45524C31U;
constexpr std::uint32_t kVersion = 1U;
constexpr std::uint32_t kMaxRecords = 10000U;
constexpr std::uint32_t kMaxCargoRecords = 100000U;
constexpr std::uint32_t kMaxHistoryRecords = 100000U;
constexpr std::uint32_t kMaxStringSize = 1'000'000U;

template <typename T>
bool writeValue(std::fstream& file, const T& value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(T));
    return static_cast<bool>(file);
}

template <typename T>
bool readValue(std::fstream& file, T& value) {
    return static_cast<bool>(file.read(reinterpret_cast<char*>(&value), sizeof(T)));
}

bool writeString(std::fstream& file, const std::string& value) {
    if (value.size() > kMaxStringSize) return false;
    const auto size = static_cast<std::uint32_t>(value.size());
    return writeValue(file, size) &&
           static_cast<bool>(file.write(value.data(), static_cast<std::streamsize>(size)));
}

bool readString(std::fstream& file, std::string& value) {
    std::uint32_t size = 0;
    if (!readValue(file, size) || size > kMaxStringSize) return false;
    value.assign(size, '\0');
    return static_cast<bool>(file.read(value.data(), static_cast<std::streamsize>(size)));
}

bool validFinite(double value) { return std::isfinite(value); }
bool validVehicleStatus(int value) { return value >= 0 && value <= static_cast<int>(VehicleStatus::Maintenance); }
bool validMissionType(int value) { return value >= 0 && value <= static_cast<int>(MissionType::Water); }
bool validMissionStatus(int value) { return value >= 0 && value <= static_cast<int>(MissionStatus::Failed); }
bool validPriority(int value) { return value >= 1 && value <= 100; }

} // namespace

bool BinaryPersistence::save(const ReliefHub& hub, const std::string& path, std::string& error) {
    error.clear();
    const std::string temporaryPath = path + ".tmp";
    std::fstream file(temporaryPath, std::ios::binary | std::ios::out | std::ios::trunc);
    auto fail = [&](const std::string& message) {
        file.close();
        std::error_code cleanupError;
        std::filesystem::remove(temporaryPath, cleanupError);
        error = message;
        return false;
    };
    if (!file) return fail("Unable to open temporary persistence file");

    if (!writeValue(file, kMagic) || !writeValue(file, kVersion)) return fail("Failed to write persistence header");
    if (hub.depots_.size() > kMaxRecords || !writeValue(file, static_cast<std::uint32_t>(hub.depots_.size()))) {
        return fail("Invalid depot count");
    }
    for (const auto& [id, depot] : hub.depots_) {
        if (id != depot.id() || !validFinite(depot.capacityKg()) || depot.capacityKg() < 0.0 ||
            !writeString(file, depot.id()) || !writeString(file, depot.name()) || !writeValue(file, depot.capacityKg())) {
            return fail("Invalid depot record");
        }
        const auto cargo = depot.inventory();
        if (cargo.size() > kMaxCargoRecords || !writeValue(file, static_cast<std::uint32_t>(cargo.size()))) {
            return fail("Invalid cargo count");
        }
        for (const auto& item : cargo) {
            if (item.id().empty() || item.quantity() <= 0 || !validFinite(item.unitWeightKg()) ||
                item.unitWeightKg() < 0.0 || !validPriority(item.priority()) ||
                !writeString(file, item.id()) || !writeString(file, item.category()) ||
                !writeValue(file, item.quantity()) || !writeValue(file, item.unitWeightKg()) ||
                !writeValue(file, item.priority())) {
                return fail("Invalid cargo record");
            }
        }
    }

    if (hub.vehicles_.size() > kMaxRecords || !writeValue(file, static_cast<std::uint32_t>(hub.vehicles_.size()))) {
        return fail("Invalid vehicle count");
    }
    for (const auto& [id, vehicle] : hub.vehicles_) {
        if (!vehicle) return fail("Invalid null vehicle record");
        const auto coordinate = vehicle->beacon().coordinate();
        const int status = static_cast<int>(vehicle->status());
        if (id != vehicle->id() || vehicle->id().empty() || vehicle->agency().empty() ||
            !validFinite(vehicle->capacityKg()) || vehicle->capacityKg() < 0.0 ||
            !validFinite(vehicle->fuelLitres()) || vehicle->fuelLitres() < 0.0 || !validVehicleStatus(status) ||
            !validFinite(coordinate.latitude) || !validFinite(coordinate.longitude) ||
            !writeString(file, vehicle->kind()) || !writeString(file, vehicle->id()) ||
            !writeString(file, vehicle->agency()) || !writeValue(file, vehicle->capacityKg()) ||
            !writeValue(file, vehicle->fuelLitres()) || !writeValue(file, status) ||
            !writeValue(file, coordinate.latitude) || !writeValue(file, coordinate.longitude)) {
            return fail("Invalid vehicle record");
        }
    }

    if (hub.missions_.size() > kMaxRecords || !writeValue(file, static_cast<std::uint32_t>(hub.missions_.size()))) {
        return fail("Invalid mission count");
    }
    for (const auto& mission : hub.missions_) {
        const int type = static_cast<int>(mission.type());
        const int status = static_cast<int>(mission.status());
        const std::uint8_t critical = mission.critical_ ? 1U : 0U;
        if (mission.id().empty() || mission.originDepot().empty() || mission.destination().empty() ||
            mission.cargoId().empty() || mission.quantity() <= 0 || !validFinite(mission.distanceKm()) ||
            mission.distanceKm() < 0.0 || !validPriority(mission.priority()) || !validMissionType(type) ||
            !validMissionStatus(status) || !writeString(file, mission.id()) || !writeValue(file, type) ||
            !writeString(file, mission.originDepot()) || !writeString(file, mission.destination()) ||
            !writeString(file, mission.cargoId()) || !writeValue(file, mission.quantity()) ||
            !writeValue(file, mission.distanceKm()) || !writeString(file, mission.corridor()) ||
            !writeValue(file, mission.priority()) || !writeValue(file, status) ||
            !writeString(file, mission.assignedVehicleId()) || !writeValue(file, critical)) {
            return fail("Invalid mission record");
        }
    }

    if (hub.history_.size() > kMaxHistoryRecords || !writeValue(file, static_cast<std::uint32_t>(hub.history_.size()))) {
        return fail("Invalid history count");
    }
    for (const auto& record : hub.history_) {
        if (!writeString(file, record)) return fail("Invalid history record");
    }
    file.flush();
    if (!file) return fail("Failed while writing persistence file");
    file.close();

    std::error_code errorCode;
    std::filesystem::rename(temporaryPath, path, errorCode);
    if (!errorCode) return true;

    errorCode.clear();
    if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
        return fail("Persistence target is a directory");
    }

    const std::string backupPath = path + ".bak";
    std::filesystem::remove(backupPath, errorCode);
    errorCode.clear();
    const bool hadOriginal = std::filesystem::exists(path);
    if (hadOriginal) {
        std::filesystem::rename(path, backupPath, errorCode);
        if (errorCode) return fail("Unable to protect existing persistence file: " + errorCode.message());
    }

    errorCode.clear();
    std::filesystem::rename(temporaryPath, path, errorCode);
    if (errorCode) {
        std::error_code restoreError;
        if (hadOriginal) std::filesystem::rename(backupPath, path, restoreError);
        return fail("Unable to replace persistence file: " + errorCode.message());
    }
    std::filesystem::remove(backupPath, errorCode);
    return true;
}

bool BinaryPersistence::load(ReliefHub& hub, const std::string& path, std::string& error) {
    error.clear();
    std::fstream file(path, std::ios::binary | std::ios::in);
    if (!file) { error = "Persistence file does not exist"; return false; }

    auto fail = [&](const std::string& message) {
        error = message;
        return false;
    };

    try {
        std::uint32_t magic = 0;
        std::uint32_t version = 0;
        if (!readValue(file, magic) || magic != kMagic || !readValue(file, version) || version != kVersion) {
            return fail("Invalid ERL-HUB persistence header");
        }

        ReliefHub loaded;
        std::uint32_t count = 0;
        if (!readValue(file, count) || count > kMaxRecords) return fail("Invalid depot count");
        for (std::uint32_t index = 0; index < count; ++index) {
            std::string id;
            std::string name;
            double capacity = 0.0;
            if (!readString(file, id) || !readString(file, name) || id.empty() || name.empty() ||
                !readValue(file, capacity) || !validFinite(capacity) || capacity < 0.0) {
                return fail("Invalid depot record");
            }
            DistributionDepot depot(id, name, capacity);
            std::uint32_t cargoCount = 0;
            if (!readValue(file, cargoCount) || cargoCount > kMaxCargoRecords) return fail("Invalid cargo count");
            for (std::uint32_t cargoIndex = 0; cargoIndex < cargoCount; ++cargoIndex) {
                std::string cargoId;
                std::string category;
                int quantity = 0;
                double unitWeight = 0.0;
                int priority = 0;
                if (!readString(file, cargoId) || !readString(file, category) || cargoId.empty() || category.empty() ||
                    !readValue(file, quantity) || quantity <= 0 || !readValue(file, unitWeight) ||
                    !validFinite(unitWeight) || unitWeight < 0.0 || !readValue(file, priority) ||
                    !validPriority(priority)) return fail("Invalid cargo record");
                depot += ReliefCargo(cargoId, category, quantity, unitWeight, priority);
            }
            if (!loaded.depots_.emplace(id, std::move(depot)).second) return fail("Duplicate depot ID");
        }

        if (!readValue(file, count) || count > kMaxRecords) return fail("Invalid vehicle count");
        for (std::uint32_t index = 0; index < count; ++index) {
            std::string kind;
            std::string id;
            std::string agency;
            double capacity = 0.0;
            double fuel = 0.0;
            int status = 0;
            double latitude = 0.0;
            double longitude = 0.0;
            if (!readString(file, kind) || !readString(file, id) || !readString(file, agency) ||
                (kind != "Ground" && kind != "Aerial" && kind != "Amphibious") || id.empty() || agency.empty() ||
                !readValue(file, capacity) || !validFinite(capacity) || capacity < 0.0 ||
                !readValue(file, fuel) || !validFinite(fuel) || fuel < 0.0 || !readValue(file, status) ||
                !validVehicleStatus(status) || !readValue(file, latitude) || !readValue(file, longitude) ||
                !validFinite(latitude) || !validFinite(longitude)) return fail("Invalid vehicle record");
            auto vehicle = makeVehicle(kind, id, agency, capacity, fuel, {latitude, longitude});
            if (!vehicle) return fail("Unable to construct vehicle record");
            vehicle->setFuelLitres(fuel);
            vehicle->setStatus(static_cast<VehicleStatus>(status));
            if (!loaded.vehicles_.emplace(id, std::move(vehicle)).second) return fail("Duplicate vehicle ID");
        }

        if (!readValue(file, count) || count > kMaxRecords) return fail("Invalid mission count");
        for (std::uint32_t index = 0; index < count; ++index) {
            std::string id;
            std::string origin;
            std::string destination;
            std::string cargoId;
            std::string corridor;
            std::string assignedVehicle;
            int type = 0;
            int quantity = 0;
            int priority = 0;
            int status = 0;
            double distance = 0.0;
            std::uint8_t critical = 0;
            if (!readString(file, id) || !readValue(file, type) || !validMissionType(type) ||
                !readString(file, origin) || !readString(file, destination) || !readString(file, cargoId) ||
                id.empty() || origin.empty() || destination.empty() || cargoId.empty() || !readValue(file, quantity) ||
                quantity <= 0 || !readValue(file, distance) || !validFinite(distance) || distance < 0.0 ||
                !readString(file, corridor) || !readValue(file, priority) || !validPriority(priority) ||
                !readValue(file, status) || !validMissionStatus(status) || !readString(file, assignedVehicle) ||
                !readValue(file, critical) || critical > 1U) return fail("Invalid mission record");
            ReliefMission mission(id, static_cast<MissionType>(type), origin, destination, cargoId,
                                  quantity, distance, corridor, priority);
            mission.status_ = static_cast<MissionStatus>(status);
            mission.assignedVehicleId_ = assignedVehicle;
            mission.critical_ = critical != 0U;
            if (!loaded.addMission(mission)) return fail("Duplicate mission ID");
            loaded.missions_.back().status_ = mission.status_;
            loaded.missions_.back().assignedVehicleId_ = mission.assignedVehicleId_;
            loaded.missions_.back().critical_ = mission.critical_;
        }

        if (!readValue(file, count) || count > kMaxHistoryRecords) return fail("Invalid history count");
        for (std::uint32_t index = 0; index < count; ++index) {
            std::string record;
            if (!readString(file, record)) return fail("Invalid history record");
            loaded.history_.push_back(std::move(record));
        }

        char trailingByte = 0;
        if (file.read(&trailingByte, 1)) return fail("Unexpected trailing data");

        for (const auto& mission : loaded.missions_) {
            const auto depot = loaded.depots_.find(mission.originDepot());
            if (depot == loaded.depots_.end() || depot->second.cargo(mission.cargoId()) == nullptr) {
                return fail("Mission references missing depot inventory");
            }
            if (!mission.assignedVehicleId().empty() && loaded.vehicles_.find(mission.assignedVehicleId()) == loaded.vehicles_.end()) {
                return fail("Mission references missing vehicle");
            }
        }

        hub = std::move(loaded);
        return true;
    } catch (const std::exception& exception) {
        error = std::string("Invalid persistence data: ") + exception.what();
        return false;
    }
}

} // namespace erl
