# Testing

The `erl_tests` executable covers:

- overloaded inventory allocation and reference semantics;
- deep-copy behavior of dynamic vehicle manifests;
- polymorphic vehicle range and virtual-inheritance construction;
- priority ordering of urgent medical missions;
- failed-dispatch atomicity;
- binary save/load round trips.

Run the suite with:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

In the current workspace, the MinGW `g++` compiler was also used directly because CMake was not installed. The complete core/test source set compiled with C++17 and `-Wall -Wextra -Wpedantic`, and the executable reported `All ERL-HUB tests passed.` The CMake/CTest command remains the intended project verification path when CMake is available.

