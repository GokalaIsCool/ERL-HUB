# Testing

The `erl_tests` executable covers:

- Unit I distance/payload utilities, both overloaded allocation paths, reference-returning operators, and friend triage override;
- Unit II encapsulated cargo/depot/vehicle values, static analytics, stream/operator round trips, and dynamic manifests;
- deep-copy behavior of dynamic vehicle manifests and destructor accounting;
- Unit III abstract-base pointers, virtual-inheritance traits, GPS/life-support composition, runtime dispatch, and virtual destruction;
- Unit IV priority queues, urgent-mission ordering, stream manipulators, exception paths, restart restoration, corrupted snapshots, invalid records, failed writes, and transactional dispatch failures;
- Route graph safe-path, vehicle-suitability and damaged-edge evaluation;
- Phase 5 end-to-end menu input/output, all six sample scenarios, persistent output after reload, and preservation of failed-operation state.

The final audit and correction record is maintained in [docs/AUDIT.md](AUDIT.md).

Run the suite with:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The strict verification command used during the audit compiled the complete source
set with C++17, `-DNDEBUG`, `-Wall -Wextra -Wpedantic` and `-Werror`; the tests still
run in Release mode because the test translation unit deliberately keeps its
assertions active. The executable reported `All ERL-HUB Unit I/II/III/IV tests passed.`

