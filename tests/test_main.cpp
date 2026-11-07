#include "erl/Mission.h"
#include "erl/PriorityDispatchQueue.h"
#include "erl/System.h"
#include "erl/Utilities.h"
#include <cassert>
#include <filesystem>
#include <iostream>

using namespace erl;

void testInventory() {
    DistributionDepot depot("D", "Depot", 1000.0);
    depot += ReliefCargo("water", "water", 10, 2.0);
    ReliefCargo allocated;
    assert(ReliefOps::Inventory::allocateSupply(depot, "water", 4, allocated));
    assert(allocated.quantity() == 4);
    assert(depot.cargo("water")->quantity() == 6);
}

void testDeepCopy() {
    GroundConvoy original("V1", "A", 100.0, 20.0);
    original += ReliefCargo("kit", "medical", 2, 5.0);
    GroundConvoy copy = original;
    copy -= ReliefCargo("kit", "medical", 1, 5.0);
    assert(original.manifestWeightKg() == 10.0);
    assert(copy.manifestWeightKg() == 5.0);
}

void testPolymorphismAndQueue() {
    std::unique_ptr<ReliefVehicle> vehicle = std::make_unique<AmphibiousCraft>("A", "R", 100, 20);
    assert(vehicle->supportsCorridor("river"));
    PriorityDispatchQueue<ReliefMission, UrgentMedicalFirst> queue;
    queue.push(ReliefMission("food", MissionType::Food, "D", "X", "c", 1, 1, "road", 3));
    queue.push(ReliefMission("med", MissionType::Medical, "D", "X", "c", 1, 1, "road", 3));
    assert(queue.pop().id() == "med");
}

void testDispatchAtomicity() {
    ReliefHub hub;
    hub.addDepot(DistributionDepot("D", "Depot", 100.0));
    hub.depot("D")->addCargo(ReliefCargo("kit", "medical", 2, 10.0));
    hub.addVehicle(makeVehicle("Ground", "V", "A", 5.0, 100.0));
    hub.addMission(ReliefMission("M", MissionType::Medical, "D", "X", "kit", 1, 10.0, "road"));
    const auto result = hub.dispatchMission("M", "V");
    assert(!result.success);
    assert(hub.depot("D")->cargo("kit")->quantity() == 2);
    assert(hub.mission("M")->status() == MissionStatus::Planned);
}

void testPersistence() {
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
    assert(ReliefOps::Logistics::payloadPercent(25, 100) == 25.0);
    testInventory();
    testDeepCopy();
    testPolymorphismAndQueue();
    testDispatchAtomicity();
    testPersistence();
    std::cout << "All ERL-HUB tests passed.\n";
}
