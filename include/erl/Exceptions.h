#pragma once

#include <exception>

namespace erl {

class CargoOverweightException : public std::exception {
public:
    const char* what() const noexcept override;
};

class FuelDepletionException : public std::exception {
public:
    const char* what() const noexcept override;
};

class DamagedCorridorException : public std::exception {
public:
    const char* what() const noexcept override;
};

} // namespace erl

