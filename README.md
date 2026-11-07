# ERL-HUB

Post-Earthquake Multi-Agency Emergency Relief Logistics Hub, implemented as a modular C++17 command-line simulation for CIC-211 (Object-Oriented Programming Using C++).

## Build

Requirements: a C++17 compiler and CMake 3.16 or newer.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Run the application from the repository root so the relative `data/` directory is available:

```bash
./build/erl_hub                 # Linux/macOS, single-config generator
# ./build/Release/erl_hub       # Visual Studio or another multi-config generator
```

On Windows, use `build\Release\erl_hub.exe` with a multi-config generator.

## Features

- Vehicle, depot, cargo and relief mission management.
- Ground, aerial and amphibious polymorphic vehicle models.
- Payload, fuel, stock and damaged-corridor validation.
- Priority dispatch queue with urgent medical ordering.
- Atomic binary persistence in `data/erl_hub.bin`.
- Demonstration scenario and tabular audit reporting.
- Selectable sample scenarios for success, overload, fuel, corridor, inventory, and critical override paths.

The application is intentionally launched from the repository root. Save/load uses `data/erl_hub.bin`; the UI creates `data/` if it is missing and reports write/load errors without changing in-memory state.

## Repository guide

- `include/erl/`: public domain, service and utility headers.
- `src/`: implementations and the interactive user interface.
- `tests/`: focused regression tests for syllabus features and dispatch atomicity.
- `docs/`: architecture, user guide, testing record and CIC-211 mapping.
- `data/`: sample input and runtime persistence location.
- `sample_outputs/`: example report output.

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md), [docs/CIC211_MAPPING.md](docs/CIC211_MAPPING.md), [docs/USER_GUIDE.md](docs/USER_GUIDE.md), and [docs/TESTING.md](docs/TESTING.md).

