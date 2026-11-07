# CIC-211 Requirement Mapping

| Requirement | Implementation |
|---|---|
| ReliefOps namespaces | `include/erl/Types.h`, `Utilities.h`, `Cargo.h`, `Depot.h` |
| Overloaded allocation | `ReliefOps::Inventory::allocateSupply` overloads |
| Reference and return-by-reference | depot allocation APIs and `ReliefHub::mission` |
| Inline utilities | `ReliefOps::Logistics::distanceKm`, `payloadPercent` |
| Friend triage | `IncidentCommander::overrideCriticalTriage` and `ReliefMission` friendship |
| Encapsulated domain classes | `ReliefCargo`, `DistributionDepot`, `TransportVehicle` |
| Static analytics | `TransportVehicle::totalRegistered`, `DistributionDepot::totalCapacity` |
| Dynamic manifest | `TransportVehicle` owns a heap array of cargo pointers |
| Deep copy/destructor | `TransportVehicle` copy constructor, assignment and destructor |
| Operators | cargo/depot/vehicle `+=`, `-=`, `==`, `<<`, `>>` |
| Abstract polymorphism | `ReliefVehicle` and `std::unique_ptr<ReliefVehicle>` |
| Virtual inheritance | `FuelPoweredVehicle`, `FloatCapableVehicle`, `AmphibiousCraft` |
| Composition | `GPSBeacon` and `EmergencyLifeSupportUnit` members |
| Exceptions | `include/erl/Exceptions.h` |
| Binary persistence | `BinaryPersistence` using `std::fstream` and atomic replacement |
| Template queue | `PriorityDispatchQueue<T>` |
| STL and functors | vectors/maps and `UrgentMedicalFirst` |
| Stream manipulators | `tableRow`, `tableSeparator` |

