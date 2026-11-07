# ERL-HUB

Post-Earthquake Multi-Agency Emergency Relief Logistics Hub, implemented as a modular C++17 command-line simulation for CIC-211 (Object-Oriented Programming Using C++).

## Build

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Run the application with:

```bash
./build/erl_hub
```

On Windows with a Visual Studio generator, use `build/Debug/erl_hub.exe`.

## Features

- Vehicle, depot, cargo and relief mission management.
- Ground, aerial and amphibious polymorphic vehicle models.
- Payload, fuel, stock and damaged-corridor validation.
- Priority dispatch queue with urgent medical ordering.
- Atomic binary persistence in `data/erl_hub.bin`.
- Demonstration scenario and tabular audit reporting.

## Repository guide

- `include/erl/`: public domain, service and utility headers.
- `src/`: implementations and the interactive user interface.
- `tests/`: focused regression tests for syllabus features and dispatch atomicity.
- `docs/`: architecture, user guide, testing record and CIC-211 mapping.
- `data/`: sample input and runtime persistence location.
- `sample_outputs/`: example report output.

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md), [docs/CIC211_MAPPING.md](docs/CIC211_MAPPING.md), [docs/USER_GUIDE.md](docs/USER_GUIDE.md), and [docs/TESTING.md](docs/TESTING.md).

