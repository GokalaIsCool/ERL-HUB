# User Guide

## First run

Build the program using the platform instructions in the repository README, then
start it from the repository root. You do not need to know C++ or configure files
by hand: the application is operated through numbered menu choices.

On the first run, choose **Load sample scenario**. The scenario menu contains
successful medical dispatch, overloaded cargo, insufficient fuel, damaged corridor,
insufficient inventory, and critical emergency override demonstrations.

## Main menu

The main menu supports:

1. List vehicles, depots and missions.
2. Load a sample scenario.
3. Create a depot.
4. Restock a depot.
5. Register a ground, aerial or amphibious vehicle.
6. Create a mission.
7. Dispatch a mission.
8. Save a binary snapshot.
9. Load a binary snapshot.
10. View runtime polymorphism output.
11. View priority dispatch order.
12. Print the dispatch audit report.

## Recommended exploration

1. Choose **2**, then load the successful medical scenario.
2. Choose **1** to inspect the depots, vehicles and missions.
3. Choose **7** to dispatch the mission.
4. Choose **8** to save the current state.
5. Choose **9** to load the saved state again.
6. Choose **12** to print the audit report.
7. Choose **0** to exit.

To observe failure handling, reload a fresh sample scenario and try the overload,
fuel, corridor or inventory scenario before dispatching it. The critical emergency
scenario demonstrates the priority override path.

## Input and persistence behavior

Enter the number shown beside a menu item and press Enter. Text fields accept spaces,
so names such as `Central Depot` and `Medical Team` are valid. Invalid numbers,
unknown vehicle types and invalid domain values are rejected without partially
creating an object.

Dispatch is transactional: validation runs before any stock, fuel, vehicle or mission
status is changed. Failed dispatches therefore do not corrupt inventory, vehicle
state or dispatch records.

Save/load uses temporary files, validates the complete binary snapshot and replaces
the in-memory hub only after a successful load. Runtime data is stored in
`data/erl_hub.bin`. The file may be deleted if you want to start with no saved
snapshot; sample scenarios can always be loaded again from the menu.

## Troubleshooting

- **`cmake` is not recognized:** install CMake and restart the terminal.
- **NMake or generator errors on Windows:** use the MinGW commands in the README
  with a fresh `build-mingw` directory, or use a Visual Studio Developer PowerShell.
- **The executable cannot find `data/`:** change directory to the repository root
  before launching it.
- **Tests are not found:** confirm that the build completed successfully before
  running `ctest`.

