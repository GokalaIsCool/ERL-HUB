#include "erl/Mission.h"
#include "erl/PriorityDispatchQueue.h"
#include "erl/System.h"
#include "erl/UserInterface.h"
#include "erl/Utilities.h"

#include <cassert>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <type_traits>

using namespace erl;

void testUnitIUtilitiesAndReferences() {
    assert(ReliefOps::Logistics::distanceKm(28.61, 77.23, 28.95, 77.10) > 0.0);
    assert(ReliefOps::Logistics::payloadPercent(25.0, 100.0) == 25.0);

    DistributionDepot depot("D", "Depot", 1000.0);
    ReliefCargo stock("water", "water", 10, 2.0);
    auto& depotReference = (depot += stock);
    assert(&depotReference == &depot);

    ReliefCargo allocated;
    assert(ReliefOps::Inventory::allocateSupply(depot, "water", 4, allocated));
    assert(allocated.quantity() == 4);
    assert(depot.cargo("water")->quantity() == 6);

    ReliefCargo source("aid", "medical", 5, 1.0);
    auto& cargoReference = (source += 0);
    assert(&cargoReference == &source);
    assert(ReliefOps::Inventory::allocateSupply(depot, source, 2, allocated));
    assert(source.quantity() == 3);

    ReliefMission mission("M", MissionType::Medical, "D", "X", "water", 1, 10.0, "road", 3);
    IncidentCommander::overrideCriticalTriage(mission, 9);
    assert(mission.critical());
    assert(mission.priority() == 1);
}

void testUnitIIValueSemanticsAndOperators() {
    ReliefCargo cargo("kit", "medical", 2, 5.0, 1);
    cargo += 1;
    cargo -= 1;
    assert(cargo.quantity() == 2);
    assert(cargo == ReliefCargo("kit", "medical", 2, 5.0, 1));

    std::stringstream cargoStream;
    cargoStream << cargo;
    ReliefCargo parsedCargo;
    cargoStream >> parsedCargo;
    assert(parsedCargo == cargo);

    DistributionDepot depot("D", "Depot", 1000.0);
    depot += cargo;
    std::stringstream depotStream;
    depotStream << depot;
    DistributionDepot parsedDepot;
    depotStream >> parsedDepot;
    assert(parsedDepot.id() == depot.id());
    depot -= ReliefCargo("kit", "medical", 1, 5.0, 1);
    assert(depot.cargo("kit")->quantity() == 1);

    GroundConvoy vehicle("V", "Agency", 100.0, 20.0);
    vehicle += cargo;
    vehicle -= ReliefCargo("kit", "medical", 1, 5.0, 1);
    assert(vehicle.manifestWeightKg() == 5.0);
    assert(vehicle == GroundConvoy("V", "Other", 1.0, 1.0));

    std::stringstream vehicleStream;
    vehicleStream << static_cast<const TransportVehicle&>(vehicle);
    TransportVehicle parsedVehicle;
    vehicleStream >> parsedVehicle;
    assert(parsedVehicle.id() == vehicle.id());
    assert(parsedVehicle.capacityKg() == vehicle.capacityKg());
}

void testDynamicManifestDeepCopyAndAnalytics() {
    const int before = TransportVehicle::totalRegistered();
    {
        GroundConvoy original("V1", "A", 100.0, 20.0);
        assert(TransportVehicle::totalRegistered() == before + 1);
        for (int index = 0; index < 6; ++index) {
            original += ReliefCargo("kit" + std::to_string(index), "medical", 1, 5.0);
        }
        GroundConvoy copy = original;
        GroundConvoy assigned("V2", "A", 100.0, 20.0);
        assigned = original;
        copy -= ReliefCargo("kit0", "medical", 1, 5.0);
        assert(original.manifestWeightKg() == 30.0);
        assert(assigned.manifestWeightKg() == 30.0);
        assert(copy.manifestWeightKg() == 25.0);
    }
    assert(TransportVehicle::totalRegistered() == before);

    const double capacityBefore = DistributionDepot::totalCapacityKg();
    {
        DistributionDepot depot("Analytics", "A", 500.0);
        assert(DistributionDepot::totalCapacityKg() == capacityBefore + 500.0);
    }
    assert(DistributionDepot::totalCapacityKg() == capacityBefore);
}

void testPolymorphismAndPriorityQueue() {
    std::unique_ptr<ReliefVehicle> vehicle = std::make_unique<AmphibiousCraft>("A", "R", 100, 20);
    assert(vehicle->supportsCorridor("river"));
    assert(!vehicle->isAirborne());

    PriorityDispatchQueue<ReliefMission, UrgentMedicalFirst> queue;
    queue.push(ReliefMission("food", MissionType::Food, "D", "X", "c", 1, 1, "road", 3));
    queue.push(ReliefMission("med", MissionType::Medical, "D", "X", "c", 1, 1, "road", 3));
    assert(queue.pop().id() == "med");
}

void testUnitIIIInheritanceAndDestruction() {
    static_assert(std::is_abstract_v<ReliefVehicle>);
    static_assert(std::is_base_of_v<ReliefVehicle, FuelPoweredVehicle>);
    static_assert(std::is_base_of_v<ReliefVehicle, FloatCapableVehicle>);

    GroundConvoy ground("G", "Agency", 100.0, 80.0, {28.61, 77.23});
    AerialTransport aerial("A", "Agency", 100.0, 80.0, {28.61, 77.23});
    AmphibiousCraft amphibious("H", "Agency", 100.0, 80.0, {28.61, 77.23});
    ReliefVehicle* groundBase = &ground;
    ReliefVehicle* aerialBase = &aerial;
    ReliefVehicle* amphibiousBase = &amphibious;

    assert(groundBase->kind() == "Ground");
    assert(aerialBase->kind() == "Aerial");
    assert(amphibiousBase->kind() == "Amphibious");
    assert(groundBase->rangeKm() == 640.0);
    assert(aerialBase->rangeKm() == 320.0);
    assert(amphibiousBase->rangeKm() == 400.0);
    assert(!groundBase->supportsCorridor("flooded"));
    assert(aerialBase->supportsCorridor("flooded"));
    assert(amphibiousBase->supportsCorridor("flooded"));
    assert(amphibiousBase->beacon().coordinate().latitude == 28.61);
    assert(amphibiousBase->lifeSupport().active());
    assert(!amphibiousBase->isAirborne());

    ReliefHub hub;
    hub.loadDemoScenario();
    const auto runtimeReport = hub.runtimeDispatchDemo();
    assert(runtimeReport.find("Ground") != std::string::npos);
    assert(runtimeReport.find("Aerial") != std::string::npos);
    assert(runtimeReport.find("Amphibious") != std::string::npos);

    const int before = TransportVehicle::totalRegistered();
    {
        std::unique_ptr<ReliefVehicle> owned = std::make_unique<AmphibiousCraft>("D", "Agency", 50.0, 10.0);
        ReliefVehicle* basePointer = owned.get();
        assert(basePointer->kind() == "Amphibious");
        assert(TransportVehicle::totalRegistered() == before + 1);
        owned.reset();
    }
    assert(TransportVehicle::totalRegistered() == before);
}

void testApplicationUsesBothAllocationPathsAndAtomicity() {
    ReliefHub hub;
    hub.loadDemoScenario();
    assert(hub.dispatchMission("M-1001", "V-101").success); // ID overload
    assert(hub.dispatchMission("M-1003", "V-303").success); // reference overload
    assert(hub.depot("D-01")->cargo("MED-KIT")->quantity() == 80);
    assert(hub.depot("D-01")->cargo("WATER")->quantity() == 400);

    ReliefHub failingHub;
    failingHub.addDepot(DistributionDepot("D", "Depot", 100.0));
    failingHub.depot("D")->addCargo(ReliefCargo("kit", "medical", 2, 10.0));
    failingHub.addVehicle(makeVehicle("Ground", "V", "A", 5.0, 100.0));
    failingHub.addMission(ReliefMission("M", MissionType::Medical, "D", "X", "kit", 1, 10.0, "road"));
    const auto result = failingHub.dispatchMission("M", "V");
    assert(!result.success);
    assert(failingHub.depot("D")->cargo("kit")->quantity() == 2);
    assert(failingHub.mission("M")->status() == MissionStatus::Planned);
}

void testPersistenceRoundTrip() {
    ReliefHub hub;
    hub.loadDemoScenario();
    std::filesystem::create_directories("data");
    const std::string path = "data/test_roundtrip.bin";
    std::string error;
    assert(hub.dispatchMission("M-1001", "V-101").success);
    assert(hub.save(path, error));
    ReliefHub restored;
    assert(restored.load(path, error));
    assert(restored.depots().size() == 1);
    assert(restored.vehicles().size() == 3);
    assert(restored.missions().size() == 3);
    assert(restored.mission("M-1001")->status() == MissionStatus::Completed);
    assert(restored.depot("D-01")->cargo("MED-KIT")->quantity() == 80);
    assert(restored.history().size() == 1);
    std::filesystem::remove(path);
}

void testUnitIVQueueSortingAndManipulators() {
    PriorityDispatchQueue<int> numbers;
    numbers.push(2);
    numbers.push(5);
    assert(numbers.size() == 2);
    assert(numbers.pop() == 5);
    assert(numbers.pop() == 2);

    ReliefHub hub;
    hub.loadDemoScenario();
    const auto priorityReport = hub.priorityDispatchReport();
    assert(priorityReport.find("Sorted: M-1001 M-1002 M-1003") != std::string::npos);
    assert(priorityReport.find("Queue: M-1001 M-1002 M-1003") != std::string::npos);
    assert(hub.auditReport().find("+----------------") != std::string::npos);
}

void testUnitIVExceptionsAndFailedTransactions() {
    bool overweightThrown = false;
    try {
        DistributionDepot depot("D", "Small", 10.0);
        depot += ReliefCargo("heavy", "equipment", 2, 10.0);
    } catch (const CargoOverweightException& exception) {
        overweightThrown = std::string(exception.what()).find("Cargo overweight") != std::string::npos;
    }
    assert(overweightThrown);

    ReliefHub hub;
    hub.loadDemoScenario();
    const int waterBefore = hub.depot("D-01")->cargo("WATER")->quantity();
    hub.addMission(ReliefMission("M-DAMAGED", MissionType::Water, "D-01", "Zone", "WATER", 10, 5.0, "damaged"));
    auto result = hub.dispatchMission("M-DAMAGED", "V-303");
    assert(!result.success);
    assert(result.message.find("Damaged corridor") != std::string::npos);
    assert(hub.depot("D-01")->cargo("WATER")->quantity() == waterBefore);
    assert(hub.mission("M-DAMAGED")->status() == MissionStatus::Planned);

    hub.addVehicle(makeVehicle("Ground", "V-LOW", "Agency", 1000.0, 0.1));
    hub.addMission(ReliefMission("M-FUEL", MissionType::Water, "D-01", "Zone", "WATER", 10, 100.0, "road"));
    result = hub.dispatchMission("M-FUEL", "V-LOW");
    assert(!result.success);
    assert(result.message.find("Fuel depletion") != std::string::npos);
    assert(hub.depot("D-01")->cargo("WATER")->quantity() == waterBefore);
    assert(hub.mission("M-FUEL")->status() == MissionStatus::Planned);

    hub.addMission(ReliefMission("M-MISSING", MissionType::Water, "D-01", "Zone", "WATER", 10000, 1.0, "road"));
    result = hub.dispatchMission("M-MISSING", "V-101");
    assert(!result.success);
    assert(result.message.find("Insufficient depot inventory") != std::string::npos);
    assert(hub.depot("D-01")->cargo("WATER")->quantity() == waterBefore);
}

void testUnitIVCorruptInvalidAndFailedWrites() {
    ReliefHub original;
    original.loadDemoScenario();
    std::filesystem::create_directories("data");
    const std::string corruptPath = "data/corrupt_snapshot.bin";
    const std::string invalidPath = "data/invalid_snapshot.bin";
    const std::string validPath = "data/valid_snapshot.bin";
    const std::string missingParentPath = "data/does-not-exist/snapshot.bin";
    std::string error;

    {
        std::fstream file(corruptPath, std::ios::binary | std::ios::out | std::ios::trunc);
        file.write("BAD", 3);
    }
    assert(!original.load(corruptPath, error));
    assert(!error.empty());
    assert(original.missions().size() == 3);

    auto writeString = [](std::fstream& file, const std::string& value) {
        const auto size = static_cast<std::uint32_t>(value.size());
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));
        file.write(value.data(), static_cast<std::streamsize>(size));
    };
    {
        std::fstream file(invalidPath, std::ios::binary | std::ios::out | std::ios::trunc);
        const std::uint32_t magic = 0x45524C31U;
        const std::uint32_t version = 1U;
        const std::uint32_t depotCount = 1U;
        const double negativeCapacity = -10.0;
        file.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
        file.write(reinterpret_cast<const char*>(&version), sizeof(version));
        file.write(reinterpret_cast<const char*>(&depotCount), sizeof(depotCount));
        writeString(file, "D-BAD");
        writeString(file, "BadDepot");
        file.write(reinterpret_cast<const char*>(&negativeCapacity), sizeof(negativeCapacity));
    }
    assert(!original.load(invalidPath, error));
    assert(error.find("Invalid depot record") != std::string::npos);
    assert(original.depots().size() == 1);

    assert(original.save(validPath, error));
    std::filesystem::remove_all("data/does-not-exist");
    assert(!original.save(missingParentPath, error));
    assert(!error.empty());
    ReliefHub restored;
    assert(restored.load(validPath, error));
    assert(restored.missions().size() == 3);

    std::filesystem::remove(corruptPath);
    std::filesystem::remove(invalidPath);
    std::filesystem::remove(validPath);
}

void testPhaseFiveScenariosAndInteractiveWorkflow() {
    struct ScenarioExpectation {
        DemoScenario scenario;
        const char* missionId;
        const char* vehicleId;
        bool shouldDispatch;
        const char* failureText;
    };
    const ScenarioExpectation expectations[] = {
        {DemoScenario::SuccessfulMedical, "M-SUCCESS", "V-SUCCESS", true, ""},
        {DemoScenario::OverloadedCargo, "M-OVERLOAD", "V-OVERLOAD", false, "Cargo overweight"},
        {DemoScenario::InsufficientFuel, "M-FUEL", "V-FUEL", false, "Fuel depletion"},
        {DemoScenario::DamagedCorridor, "M-DAMAGE", "V-DAMAGE", false, "Damaged corridor"},
        {DemoScenario::InsufficientInventory, "M-STOCK", "V-STOCK", false, "Insufficient depot inventory"},
    };

    for (const auto& expectation : expectations) {
        ReliefHub hub;
        hub.loadScenario(expectation.scenario);
        const auto stockBefore = hub.depots().begin()->second.currentWeightKg();
        const auto result = hub.dispatchMission(expectation.missionId, expectation.vehicleId);
        assert(result.success == expectation.shouldDispatch);
        if (!expectation.shouldDispatch) {
            assert(result.message.find(expectation.failureText) != std::string::npos);
            assert(hub.depots().begin()->second.currentWeightKg() == stockBefore);
            assert(hub.mission(expectation.missionId)->status() == MissionStatus::Planned);
            assert(hub.vehicle(expectation.vehicleId)->status() == VehicleStatus::Available);
            assert(hub.history().empty());
        } else {
            assert(hub.mission(expectation.missionId)->status() == MissionStatus::Completed);
            assert(hub.history().size() == 1);
        }
    }

    ReliefHub criticalHub;
    criticalHub.loadScenario(DemoScenario::CriticalEmergency);
    assert(criticalHub.mission("M-CRITICAL")->critical());
    assert(criticalHub.mission("M-CRITICAL")->priority() == 1);
    assert(criticalHub.dispatchMission("M-CRITICAL", "V-CRITICAL").success);

    ReliefHub interactiveHub;
    std::istringstream input("2\n1\n7\nM-SUCCESS\nV-SUCCESS\n8\n9\n12\n0\n");
    std::ostringstream output;
    runInteractive(interactiveHub, input, output);
    const auto transcript = output.str();
    assert(transcript.find("Loaded: Successful medical dispatch") != std::string::npos);
    assert(transcript.find("SUCCESS:") != std::string::npos);
    assert(transcript.find("Saved.") != std::string::npos);
    assert(transcript.find("Loaded.") != std::string::npos);
    assert(transcript.find("ERL-HUB DISPATCH AUDIT") != std::string::npos);
    std::filesystem::remove("data/erl_hub.bin");
}

int main() {
    testUnitIUtilitiesAndReferences();
    testUnitIIValueSemanticsAndOperators();
    testDynamicManifestDeepCopyAndAnalytics();
    testPolymorphismAndPriorityQueue();
    testUnitIIIInheritanceAndDestruction();
    testApplicationUsesBothAllocationPathsAndAtomicity();
    testPersistenceRoundTrip();
    testUnitIVQueueSortingAndManipulators();
    testUnitIVExceptionsAndFailedTransactions();
    testUnitIVCorruptInvalidAndFailedWrites();
    testPhaseFiveScenariosAndInteractiveWorkflow();
    std::cout << "All ERL-HUB Unit I/II/III/IV tests passed.\n";
}
