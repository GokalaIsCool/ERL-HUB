# ERL-HUB

## Post-Earthquake Multi-Agency Emergency Relief Logistics Hub

ERL-HUB is a C++17 command-line simulation for coordinating emergency relief
missions after an earthquake. It models depots, relief cargo, heterogeneous vehicles,
missions, route checks, transactional dispatch, priority ordering and binary
save/load.

This is an academic simulation for CIC-211. It does not connect to live vehicles,
GPS services, maps, agencies or external emergency-management systems.

## Project contributors

| Contributor | Enrollment number | College | Main contribution |
|---|---|---|---|
| Abhishek Narayan Padhi | `00896402725` | Maharaja Agrasen Institute of Technology | Project setup and architecture, Unit III inheritance and polymorphism, Unit IV persistence, exceptions, STL and integration. |
| Ashish Thakur | `02496402725` | Maharaja Agrasen Institute of Technology | Unit I foundations and Unit II classes, encapsulation, operators, dynamic memory and deep-copy work. |

The contribution split above records the team allocation used for this submission.

## What is implemented

- Ground, aerial and amphibious relief vehicles with runtime polymorphism.
- Encapsulated cargo, depot, vehicle and mission models with validation.
- A small route graph containing safe road, highway, urban, river and flooded paths,
  plus explicitly damaged paths.
- Dispatch validation for inventory, payload, fuel, vehicle suitability and route
  safety before state changes are committed.
- Critical medical priority override and a generic priority dispatch queue.
- Atomic binary snapshots at `data/erl_hub.bin`; fields are serialized individually,
  never by dumping C++ objects containing pointers or strings.
- Corrupt-file, invalid-record, failed-write and failed-dispatch protection.
- A menu-driven interface, sample scenarios and an audit report.
- Tests covering the CIC-211 requirements and the integrated workflows.

## Quick start

You need a C++17 compiler and CMake 3.16 or newer. Git is only needed when cloning
or contributing; the program itself does not require Git.

### Linux or macOS

Run from the repository root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/erl_hub
```

### Windows with MSYS2 MinGW-w64

The following commands match an MSYS2 UCRT64 installation. If your compiler is in a
different location, find it with `where.exe g++` and change the compiler path.

```powershell
cmake -S . -B build-mingw -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER="C:\msys64\ucrt64\bin\g++.exe"
cmake --build build-mingw --parallel
ctest --test-dir build-mingw --output-on-failure
.\build-mingw\erl_hub.exe
```

### Windows with Visual Studio

Use a Developer PowerShell for Visual Studio:

```powershell
cmake -S . -B build-vs -G "Visual Studio 17 2022" -A x64
cmake --build build-vs --config Release --parallel
ctest --test-dir build-vs -C Release --output-on-failure
.\build-vs\Release\erl_hub.exe
```

Use a fresh build directory when changing CMake generators. A directory configured
for NMake cannot be reused as a MinGW directory.

## First session

The program is intentionally operated through numbered choices. A useful first run
is:

1. Choose `2` — Load sample scenario.
2. Choose scenario `1` — Successful medical dispatch.
3. Choose `1` — Inspect the loaded network.
4. Choose `7`, then enter `M-SUCCESS` and `V-SUCCESS`.
5. Choose `8` — Save the snapshot.
6. Choose `9` — Load the snapshot again.
7. Choose `12` — Print the audit report.
8. Choose `0` — Exit.

Names and agencies may contain spaces. Invalid values are rejected without partially
creating a depot, vehicle, cargo record or mission.

For a non-interactive tour, the repository includes `data/demo_input.txt`. From the
repository root, use shell redirection rather than piping line objects from
PowerShell:

```powershell
cmd /c ".\build-mingw\erl_hub.exe < data\demo_input.txt"
```

## Menu

| Choice | Operation |
|---:|---|
| 1 | List depots, vehicles and missions |
| 2 | Load one of six sample scenarios |
| 3 | Create a depot |
| 4 | Restock a depot |
| 5 | Register a ground, aerial or amphibious vehicle |
| 6 | Create a mission |
| 7 | Dispatch a mission using a selected vehicle |
| 8 | Save `data/erl_hub.bin` |
| 9 | Load `data/erl_hub.bin` |
| 10 | Show runtime polymorphism output |
| 11 | Show priority dispatch ordering |
| 12 | Print the complete audit report |
| 0 | Exit |

## Sample scenarios

Each scenario loads a fresh, deterministic state. Dispatch the displayed mission with
the displayed vehicle to observe the corresponding result.

| Scenario | Mission | Vehicle | Expected result |
|---|---|---|---|
| Successful medical dispatch | `M-SUCCESS` | `V-SUCCESS` | Dispatch succeeds and mission becomes completed |
| Overloaded cargo | `M-OVERLOAD` | `V-OVERLOAD` | Fails with cargo-overweight validation |
| Insufficient fuel | `M-FUEL` | `V-FUEL` | Fails with fuel-depletion validation |
| Damaged corridor | `M-DAMAGE` | `V-DAMAGE` | Fails with damaged-corridor validation |
| Insufficient inventory | `M-STOCK` | `V-STOCK` | Fails with inventory validation |
| Critical emergency override | `M-CRITICAL` | `V-CRITICAL` | Priority is raised to 1 and dispatch succeeds |

Failed dispatches leave inventory, fuel, vehicle status, mission status and dispatch
history unchanged.

## Persistence

The application saves a versioned binary snapshot to `data/erl_hub.bin`. The `data/`
directory is created when saving if necessary. Save and load operations use temporary
files, validate every field and replace the previous snapshot only after the complete
operation succeeds. Loading a bad file does not replace the current in-memory state.

The binary format writes individual primitive and string fields. It does not write
raw `ReliefHub`, `std::string`, `std::vector` or pointer memory to disk.

## Repository structure

```text
ERL-HUB/
├── .github/workflows/build.yml  # Ubuntu, macOS and Windows CI build matrix
├── CMakeLists.txt               # C++17 targets and CTest registration
├── README.md                    # Project overview and quick start
├── .gitignore
├── include/erl/                 # Public headers and class interfaces
│   ├── Cargo.h
│   ├── Depot.h
│   ├── Dispatch.h
│   ├── Exceptions.h
│   ├── Formatters.h
│   ├── Mission.h
│   ├── Persistence.h
│   ├── PriorityDispatchQueue.h
│   ├── Route.h
│   ├── System.h
│   ├── Types.h
│   ├── UserInterface.h
│   ├── Utilities.h
│   └── Vehicle.h
├── src/                         # Implementations and application entry point
├── tests/test_main.cpp          # Regression and integration tests
├── docs/                        # Architecture, mapping, audit, testing and guide
├── data/demo_input.txt          # Optional scripted menu input
└── sample_outputs/              # Example report output
```

## Documentation

- [User Guide](docs/USER_GUIDE.md) — guided menu exploration and custom workflows.
- [Architecture](docs/ARCHITECTURE.md) — module boundaries and ownership.
- [CIC-211 Mapping](docs/CIC211_MAPPING.md) — requirement-to-code mapping.
- [Testing](docs/TESTING.md) — test coverage and verification commands.
- [Audit](docs/AUDIT.md) — implementation audit and hardening record.

## Scope and limitations

Route planning is a deterministic in-memory graph used for simulation. Coordinates
are stored as vehicle beacon data; they are not used to query a real map. Persistence
is local to the chosen working directory, and there is no network service or
multi-user synchronization.

## Academic context

ERL-HUB was developed for CIC-211, Object-Oriented Programming Using C++, using
namespaces, overloads, references, friends, encapsulation, dynamic memory, deep
copying, operator overloads, inheritance, virtual inheritance, polymorphism,
exceptions, binary streams, STL containers, sorting functors and custom manipulators.
