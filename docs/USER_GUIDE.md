# ERL-HUB User Guide

This guide is for someone who has downloaded the repository and wants to explore the
program without needing to understand its C++ implementation.

## Start here

1. Follow the platform build commands in the [README](../README.md).
2. Run the executable from the repository root.
3. At the menu, enter one number and press Enter.
4. If you make a mistake, read the message and choose the next menu option; the
   program keeps the current state unless an operation succeeds.

The first-time tour is:

```text
2  -> Load sample scenario
1  -> Successful medical dispatch
1  -> List the network
7  -> Dispatch M-SUCCESS with V-SUCCESS
8  -> Save snapshot
9  -> Load snapshot
12 -> Print audit report
0  -> Exit
```

When entering this interactively, type each value only when its prompt appears.

## What each menu choice does

| Choice | What to try |
|---:|---|
| 1 | See current depot stock, vehicle fuel/status and mission status. |
| 2 | Reset the in-memory hub with a deterministic sample scenario. |
| 3 | Add a depot such as `D-USER`, `Community Depot`, capacity `1000`. |
| 4 | Add cargo to an existing depot. |
| 5 | Register a Ground, Aerial or Amphibious vehicle. |
| 6 | Create a mission using a depot, cargo ID, distance and corridor. |
| 7 | Attempt a dispatch and see the validation result. |
| 8 | Save the current hub to `data/erl_hub.bin`. |
| 9 | Restore the saved hub after it has been changed or after restarting. |
| 10 | Observe derived vehicle behavior through base-class references. |
| 11 | Compare sorted mission order with the priority queue order. |
| 12 | Print a combined runtime, queue, mission and inventory audit. |
| 0 | Close the application. |

## Scenario tour

Choose menu option `2`, then the scenario number. After it loads, choose menu option
`7` and enter the matching mission and vehicle IDs below.

| Scenario number | Mission ID | Vehicle ID | What it demonstrates |
|---:|---|---|---|
| 1 | `M-SUCCESS` | `V-SUCCESS` | Normal medical dispatch succeeds. |
| 2 | `M-OVERLOAD` | `V-OVERLOAD` | Payload capacity rejects an overweight load. |
| 3 | `M-FUEL` | `V-FUEL` | Fuel validation rejects an out-of-range trip. |
| 4 | `M-DAMAGE` | `V-DAMAGE` | The route graph rejects a damaged corridor. |
| 5 | `M-STOCK` | `V-STOCK` | The depot rejects a request larger than stock. |
| 6 | `M-CRITICAL` | `V-CRITICAL` | Friend-based triage raises priority to 1. |

After a failed scenario, choose option `1` to confirm that stock, fuel, mission
status, vehicle status and history were not changed. Load another scenario to reset
the hub for a clean experiment.

## Create your own successful mission

The following is a simple manual exercise using the numbered menu:

1. Choose `3` and enter:
   - Depot ID: `D-USER`
   - Depot name: `Community Depot`
   - Capacity: `1000`
2. Choose `4` and enter:
   - Depot ID: `D-USER`
   - Cargo ID: `water`
   - Category: `water`
   - Quantity: `100`
   - Unit weight: `1`
   - Priority: `2`
3. Choose `5` and enter:
   - Kind: `Ground`
   - Vehicle ID: `V-USER`
   - Agency: `Volunteer Team`
   - Payload capacity: `500`
   - Fuel: `100`
4. Choose `6` and enter:
   - Mission ID: `M-USER`
   - Type: `Water`
   - Origin depot: `D-USER`
   - Destination: `Clinic`
   - Cargo ID: `water`
   - Quantity: `10`
   - Distance: `20`
   - Corridor: `road`
   - Priority: `2`
5. Choose `7`, then dispatch `M-USER` with `V-USER`.

The route graph will evaluate a safe road path, then dispatch validates inventory,
capacity and fuel before committing the operation.

## Persistence experiment

1. Load a scenario and dispatch it.
2. Choose `8` and wait for `Saved.`.
3. Exit with `0`.
4. Start the executable again from the repository root.
5. Choose `9` and wait for `Loaded.`.
6. Choose `1` or `12` to inspect the restored mission, inventory and history.

The snapshot is stored at `data/erl_hub.bin`. It is generated runtime data and is
not required to clone or build the project.

## Input rules and safety

- Text values can contain spaces, for example `Central Depot`.
- Mission types are `Medical`, `Shelter`, `Rescue`, `Food` and `Water`.
- Supported interactive corridor names are `road`, `highway`, `urban`, `river`,
  `flooded` and `damaged`.
- Priority values range from 1 (highest) to 100.
- Negative quantities, invalid capacities, unknown vehicle kinds and malformed
  snapshots are rejected.
- A failed dispatch does not partially consume inventory or fuel.

## Troubleshooting

**CMake selects NMake and says `nmake` is missing.** Use the Windows MinGW commands
from the README with `-G "MinGW Makefiles"` and a new `build-mingw` directory, or
use a Visual Studio Developer PowerShell.

**CTest says the test is `Not Run`.** The test executable was not built. Run
`cmake --build ...` successfully before running `ctest`.

**The executable cannot find or save `data/erl_hub.bin`.** Start it from the
repository root, not from inside the build directory.

**A menu value is rejected.** Enter the requested value at the current prompt. For
numeric prompts, use a plain number and press Enter.
