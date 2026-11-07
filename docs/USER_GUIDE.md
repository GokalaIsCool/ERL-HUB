# User Guide

Start the program from the repository root and choose **Load sample scenario**. The scenario menu contains successful medical dispatch, overloaded cargo, insufficient fuel, damaged corridor, insufficient inventory, and critical emergency override demonstrations. The main menu supports:

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

Dispatch is transactional: validation runs before any stock, fuel, vehicle or mission status is changed. Invalid numeric input is rejected and the menu is shown again. Save/load uses temporary files and validates the complete snapshot before replacing the in-memory hub.

