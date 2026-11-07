# CIC-211 Completion Audit

## Requirement status

| Area | Status | Evidence |
|---|---|---|
| Unit I namespaces/utilities | Complete | `Utilities.h`, `Depot.h`, `Dispatch.cpp`, `System.cpp` |
| Unit I allocation/reference semantics | Complete | Both allocation overloads are selected by real dispatches; reference-returning operators are chained and tested |
| Unit I friend triage | Complete | `IncidentCommander` changes critical missions and is used in sample scenarios |
| Unit II domain classes/encapsulation | Complete | `Cargo`, `DistributionDepot`, and `TransportVehicle` with const accessors and validation |
| Unit II dynamic ownership/deep copy | Complete | Heap manifest, copy constructor, copy-and-swap assignment, destructor, and live-count tests |
| Unit II operators | Complete | Cargo, depot, and vehicle arithmetic, comparison, and stream round trips |
| Unit III inheritance/composition | Complete | Abstract base pointers, virtual hybrid branches, GPS beacon, life support, and destruction tests |
| Route planning | Complete | `RouteGraph` evaluates safe and damaged paths and is called by every dispatch |
| Unit IV exceptions/persistence | Complete | Validated binary snapshots, atomic replacement, rollback, corruption/invalid-data tests |
| Unit IV queue/sorting/I/O | Complete | Generic queue, urgent mission ordering, table manipulators, and application reports |
| Phase 5 integration | Complete | Menu-driven scenarios, dispatch, save/reload, and injected I/O workflow tests |

## Hardening corrections made

- Made `TransportVehicle` copy assignment strongly exception-safe using copy-and-swap.
- Added cleanup for partially constructed deep-copy manifests.
- Rejected invalid, non-finite, empty, or unknown domain values at construction and stream boundaries.
- Prevented unknown vehicle kinds from silently becoming ground vehicles.
- Made interactive text input line-based so names and agencies may contain spaces.
- Added UI exception handling for invalid depot and vehicle creation.
- Added fixed-width boolean persistence and stricter snapshot validation.
- Made persistence tests independent of the process working directory.
- Added a real deterministic route graph with safe checkpoints and damaged edges.
- Replaced the inaccurate menu/tree/example documentation with the implemented
  menu, executable names, workflows and limitations.
- Replaced the placeholder scripted-input file with a runnable end-to-end tour.

## Verification

The project was compiled with C++17, `-DNDEBUG`, `-Wall -Wextra -Wpedantic` and
`-Werror`. The complete test executable passed all Unit I–IV, route graph,
integration, persistence, invalid-input, scenario and menu workflow tests. A
scripted menu run also completed load, list, dispatch, save, reload and audit output.
The CMake target now links the UI implementation into `erl_tests`, which prevents
the undefined `runInteractive` failure on MinGW.
