# ERL-HUB Architecture

## Design

ERL-HUB is split into four layers:

1. **Domain** (`Cargo`, `DistributionDepot`, `TransportVehicle`, the `ReliefVehicle` hierarchy and `ReliefMission`) owns state and validates local invariants.
2. **Operations** (`DispatchCoordinator` and `ReliefHub`) applies cross-object rules such as inventory, vehicle capacity, fuel and corridor safety.
3. **Persistence** (`BinaryPersistence`) serializes a complete snapshot and commits it through a temporary file followed by replacement, so a failed save does not corrupt the previous snapshot.
4. **Presentation** (`UserInterface` and stream formatters) handles menus, input validation and reports without embedding business rules.

`ReliefHub` is the application façade. It owns depots and vehicles using `std::unordered_map`, missions in a `std::vector`, and dispatch history in a vector. `ReliefVehicle` pointers are used for runtime polymorphism while ownership remains with `std::unique_ptr`.

## Repository structure

```text
include/erl/       headers and templates
src/               implementations and main program
tests/             executable regression tests
docs/              architecture and user-facing documentation
data/              sample input and runtime data location
sample_outputs/    representative reports
```

## Class relationship diagram

```text
ReliefCargo  <------ DistributionDepot ------> ReliefHub
                         |                         |
                         | stocks                  | owns
                         v                         v
                    ReliefMission <------ DispatchCoordinator
                         |                         |
                         | assigned               | observes
                         v                         v
                    ReliefVehicle (abstract)   PriorityDispatchQueue<T>
                       /       |        \
                      /        |         \
             GroundConvoy  AerialTransport  AmphibiousCraft
                 |               |          /             \
                 +---------------+---------/               \
               FuelPoweredVehicle   FloatCapableVehicle
                         \           /
                          virtual ReliefVehicle

ReliefVehicle *composes* GPSBeacon and EmergencyLifeSupportUnit.
IncidentCommander is a friend of ReliefMission and performs critical triage override.
```

## Implementation and testing phases

1. Domain model and compile-only integration.
2. Dispatch validation, priority queue and report formatting.
3. Binary persistence and demo scenario.
4. Interactive menu, regression tests and documentation.

Each phase is built with CMake, run through CTest, and checked with warnings enabled.

