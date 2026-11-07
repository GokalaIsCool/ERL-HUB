#pragma once
#include <exception>
#include <string>
namespace erl {
class CargoOverweightException : public std::exception { public: explicit CargoOverweightException(double requested, double capacity); const char* what() const noexcept override; private: std::string message_; };
class FuelDepletionException : public std::exception { public: explicit FuelDepletionException(double required, double available); const char* what() const noexcept override; private: std::string message_; };
class DamagedCorridorException : public std::exception { public: explicit DamagedCorridorException(const std::string& corridor); const char* what() const noexcept override; private: std::string message_; };
} // namespace erl

