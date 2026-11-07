# ERL-HUB

Post-Earthquake Multi-Agency Emergency Relief Logistics Hub, implemented as a modular C++17 command-line simulation for CIC-211 (Object-Oriented Programming Using C++).

## Build

Requirements: a C++17 compiler and CMake 3.16 or newer. The commands below use only
standard command-line tools; no IDE-specific project setup is required.

### Linux or macOS

From the repository root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/erl_hub
```

### Windows with MinGW-w64/MSYS2

PowerShell must be able to find both `g++` and `mingw32-make`. If they are installed
in the standard MSYS2 UCRT64 location, use:

```powershell
cmake -S . -B build-mingw -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER="C:\msys64\ucrt64\bin\g++.exe"
cmake --build build-mingw --parallel
ctest --test-dir build-mingw --output-on-failure
.\build-mingw\erl_hub.exe
```

If `g++` is installed elsewhere, find its location with `where.exe g++` and replace
the compiler path. Use a new build directory when changing CMake generators; a
directory previously configured for NMake or Visual Studio cannot be reused for
MinGW without clearing its generated cache.

### Windows with Visual Studio

Run these commands from a Developer PowerShell for Visual Studio:

```powershell
cmake -S . -B build-vs -G "Visual Studio 17 2022" -A x64
cmake --build build-vs --config Release --parallel
ctest --test-dir build-vs -C Release --output-on-failure
.\build-vs\Release\erl_hub.exe
```

Always launch the application from the repository root so the relative `data/`
directory is available. Save/load uses `data/erl_hub.bin`.

## Features

- Vehicle, depot, cargo and relief mission management.
- Ground, aerial and amphibious polymorphic vehicle models.
- Payload, fuel, stock and damaged-corridor validation.
- Priority dispatch queue with urgent medical ordering.
- Atomic binary persistence in `data/erl_hub.bin`.
- Demonstration scenario and tabular audit reporting.
- Selectable sample scenarios for success, overload, fuel, corridor, inventory, and critical override paths.

The UI creates `data/` if it is missing and reports write/load errors without changing
in-memory state.

## Repository guide

- `include/erl/`: public domain, service and utility headers.
- `src/`: implementations and the interactive user interface.
- `tests/`: focused regression tests for syllabus features and dispatch atomicity.
- `docs/`: architecture, user guide, testing record and CIC-211 mapping.
- `data/`: sample input and runtime persistence location.
- `sample_outputs/`: example report output.

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md), [docs/CIC211_MAPPING.md](docs/CIC211_MAPPING.md), [docs/USER_GUIDE.md](docs/USER_GUIDE.md), [docs/TESTING.md](docs/TESTING.md), and [docs/AUDIT.md](docs/AUDIT.md).

