cmake_minimum_required(VERSION 3.16)
project(erl_hub LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

add_library(erl_core
    src/Types.cpp
    src/Exceptions.cpp
    src/Cargo.cpp
    src/Depot.cpp
    src/Vehicle.cpp
    src/Mission.cpp
    src/Dispatch.cpp
    src/Persistence.cpp
    src/System.cpp
    src/Formatters.cpp
)

target_include_directories(erl_core PUBLIC include)
if(MSVC)
    target_compile_options(erl_core PRIVATE /W4)
else()
    target_compile_options(erl_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(erl_hub src/main.cpp src/UserInterface.cpp)
target_link_libraries(erl_hub PRIVATE erl_core)
if(MSVC)
    target_compile_options(erl_hub PRIVATE /W4)
else()
    target_compile_options(erl_hub PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_executable(erl_tests tests/test_main.cpp src/UserInterface.cpp)
target_link_libraries(erl_tests PRIVATE erl_core)
if(MSVC)
    target_compile_options(erl_tests PRIVATE /W4)
else()
    target_compile_options(erl_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()
add_test(NAME erl_tests COMMAND erl_tests)

install(TARGETS erl_hub RUNTIME DESTINATION bin)

