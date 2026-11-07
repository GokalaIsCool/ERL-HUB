#include "erl/UserInterface.h"

#include "erl/Mission.h"
#include "erl/System.h"

#include <array>
#include <filesystem>
#include <iostream>
#include <limits>
#include <string>
#include <utility>

namespace erl {
namespace {

template <typename T>
T readNumber(const std::string& prompt, std::istream& input, std::ostream& output, T fallback = {}) {
    while (true) {
        output << prompt;
        T value{};
        if (input >> value) return value;
        if (input.eof()) return fallback;
        input.clear();
        input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        output << "Invalid input. Please enter a number.\n";
    }
}

std::string readText(const std::string& prompt, std::istream& input, std::ostream& output) {
    output << prompt;
    std::string value;
    input >> std::ws;
    std::getline(input, value);
    return value;
}

void listEntities(const ReliefHub& hub, std::ostream& output) {
    output << "\nDepots:\n";
    for (const auto& [id, depot] : hub.depots()) {
        output << "  " << id << " - " << depot.name() << " ("
               << depot.currentWeightKg() << "/" << depot.capacityKg() << " kg)\n";
    }
    output << "Vehicles:\n";
    for (const auto& [id, vehicle] : hub.vehicles()) {
        output << "  " << id << " - " << vehicle->kind() << " ("
               << toString(vehicle->status()) << ", " << vehicle->fuelLitres() << " L)\n";
    }
    output << "Missions:\n";
    for (const auto& mission : hub.missions()) {
        output << "  " << mission.id() << " - " << toString(mission.type()) << " ("
               << toString(mission.status()) << ")\n";
    }
}

void loadScenarioMenu(ReliefHub& hub, std::istream& input, std::ostream& output) {
    constexpr std::array scenarios{
        DemoScenario::SuccessfulMedical,
        DemoScenario::OverloadedCargo,
        DemoScenario::InsufficientFuel,
        DemoScenario::DamagedCorridor,
        DemoScenario::InsufficientInventory,
        DemoScenario::CriticalEmergency};
    output << "\nSAMPLE SCENARIOS\n";
    for (std::size_t index = 0; index < scenarios.size(); ++index) {
        output << (index + 1) << ". " << scenarioName(scenarios[index]) << '\n';
    }
    output << "0. Cancel\n";
    const int choice = readNumber<int>("Scenario: ", input, output);
    if (choice < 1 || choice > static_cast<int>(scenarios.size())) return;
    hub.loadScenario(scenarios[static_cast<std::size_t>(choice - 1)]);
    output << "Loaded: " << scenarioName(scenarios[static_cast<std::size_t>(choice - 1)]) << "\n";
}

void createDepot(ReliefHub& hub, std::istream& input, std::ostream& output) {
    const auto id = readText("Depot ID: ", input, output);
    const auto name = readText("Depot name: ", input, output);
    const auto capacity = readNumber<double>("Capacity kg: ", input, output);
    try {
        output << (hub.addDepot(DistributionDepot(id, name, capacity))
                       ? "Depot created.\n"
                       : "ID already exists.\n");
    } catch (const std::exception& exception) {
        output << exception.what() << "\n";
    }
}

void restock(ReliefHub& hub, std::istream& input, std::ostream& output) {
    auto* depot = hub.depot(readText("Depot ID: ", input, output));
    if (!depot) { output << "Depot not found.\n"; return; }
    const auto cargoId = readText("Cargo ID: ", input, output);
    const auto category = readText("Category: ", input, output);
    const auto quantity = readNumber<int>("Quantity: ", input, output);
    const auto weight = readNumber<double>("Unit weight kg: ", input, output);
    const auto priority = readNumber<int>("Priority (1 highest): ", input, output, 3);
    try {
        *depot += ReliefCargo(cargoId, category, quantity, weight, priority);
        output << "Restocked.\n";
    } catch (const std::exception& exception) {
        output << exception.what() << "\n";
    }
}

void registerVehicle(ReliefHub& hub, std::istream& input, std::ostream& output) {
    const auto kind = readText("Vehicle kind (Ground/Aerial/Amphibious): ", input, output);
    const auto id = readText("Vehicle ID: ", input, output);
    const auto agency = readText("Agency: ", input, output);
    const auto capacity = readNumber<double>("Payload capacity kg: ", input, output);
    const auto fuel = readNumber<double>("Fuel litres: ", input, output);
    try {
        auto vehicle = makeVehicle(kind, id, agency, capacity, fuel);
        output << (hub.addVehicle(std::move(vehicle))
                       ? "Vehicle registered.\n"
                       : "Vehicle data invalid or ID already exists.\n");
    } catch (const std::exception& exception) {
        output << exception.what() << "\n";
    }
}

void createMission(ReliefHub& hub, std::istream& input, std::ostream& output) {
    const auto id = readText("Mission ID: ", input, output);
    const auto typeText = readText("Type (Medical/Shelter/Rescue/Food/Water): ", input, output);
    const auto origin = readText("Origin depot ID: ", input, output);
    const auto destination = readText("Destination: ", input, output);
    const auto cargoId = readText("Cargo ID: ", input, output);
    const auto quantity = readNumber<int>("Quantity: ", input, output);
    const auto distance = readNumber<double>("Distance km: ", input, output);
    const auto corridor = readText("Corridor (road/river/flooded/damaged): ", input, output);
    const auto priority = readNumber<int>("Priority (1 highest): ", input, output, 3);
    try {
        if (!isMissionTypeString(typeText)) {
            output << "Unknown mission type.\n";
            return;
        }
        const auto type = missionTypeFromString(typeText);
        output << (hub.addMission(ReliefMission(id, type, origin, destination, cargoId,
                                                 quantity, distance, corridor, priority))
                       ? "Mission created.\n"
                       : "ID already exists.\n");
    } catch (const std::exception& exception) {
        output << exception.what() << "\n";
    }
}

} // namespace

void runInteractive(ReliefHub& hub) {
    runInteractive(hub, std::cin, std::cout);
}

void runInteractive(ReliefHub& hub, std::istream& input, std::ostream& output) {
    while (true) {
        output << "\nERL-HUB MENU\n"
               << "1. List network\n2. Load sample scenario\n3. Create depot\n"
               << "4. Restock depot\n5. Register vehicle\n6. Create mission\n"
               << "7. Dispatch mission\n8. Save snapshot\n9. Load snapshot\n"
               << "10. Runtime polymorphism demo\n11. Priority dispatch report\n"
               << "12. Print audit report\n0. Exit\n";
        const int choice = readNumber<int>("Choice: ", input, output);
        if (choice == 0) return;
        if (choice == 1) listEntities(hub, output);
        else if (choice == 2) loadScenarioMenu(hub, input, output);
        else if (choice == 3) createDepot(hub, input, output);
        else if (choice == 4) restock(hub, input, output);
        else if (choice == 5) registerVehicle(hub, input, output);
        else if (choice == 6) createMission(hub, input, output);
        else if (choice == 7) {
            const auto missionId = readText("Mission ID: ", input, output);
            const auto vehicleId = readText("Vehicle ID: ", input, output);
            const auto result = hub.dispatchMission(missionId, vehicleId);
            output << (result.success ? "SUCCESS: " : "FAILED: ") << result.message << '\n';
        } else if (choice == 8) {
            std::error_code directoryError;
            std::filesystem::create_directories("data", directoryError);
            std::string error;
            output << (hub.save("data/erl_hub.bin", error) ? "Saved.\n" : error + "\n");
        } else if (choice == 9) {
            std::string error;
            output << (hub.load("data/erl_hub.bin", error) ? "Loaded.\n" : error + "\n");
        } else if (choice == 10) output << hub.runtimeDispatchDemo();
        else if (choice == 11) output << hub.priorityDispatchReport();
        else if (choice == 12) output << hub.auditReport();
        else output << "Unknown menu choice.\n";
    }
}

} // namespace erl
