#include "erl/Mission.h"
#include "erl/PriorityDispatchQueue.h"
#include "erl/System.h"
#include "erl/Utilities.h"

#include <cassert>
#include <filesystem>
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
    const std::string path = "data/test_roundtrip.bin";
    std::string error;
    assert(hub.save(path, error));
    ReliefHub restored;
    assert(restored.load(path, error));
    assert(restored.depots().size() == 1);
    assert(restored.vehicles().size() == 3);
    assert(restored.missions().size() == 3);
    std::filesystem::remove(path);
}

int main() {
    testUnitIUtilitiesAndReferences();
    testUnitIIValueSemanticsAndOperators();
    testDynamicManifestDeepCopyAndAnalytics();
    testPolymorphismAndPriorityQueue();
    testUnitIIIInheritanceAndDestruction();
    testApplicationUsesBothAllocationPathsAndAtomicity();
    testPersistenceRoundTrip();
    std::cout << "All ERL-HUB Unit I/II/III tests passed.\n";
}
