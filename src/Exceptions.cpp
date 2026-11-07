#include "erl/Exceptions.h"
#include <string>
namespace erl {
CargoOverweightException::CargoOverweightException(double r,double c):message_("Cargo overweight: "+std::to_string(r)+" kg requested, capacity is "+std::to_string(c)+" kg"){} const char*CargoOverweightException::what()const noexcept{return message_.c_str();}
FuelDepletionException::FuelDepletionException(double r,double a):message_("Fuel depletion: "+std::to_string(r)+" litres required, "+std::to_string(a)+" available"){} const char*FuelDepletionException::what()const noexcept{return message_.c_str();}
DamagedCorridorException::DamagedCorridorException(const std::string&c):message_("Damaged corridor: "+c){} const char*DamagedCorridorException::what()const noexcept{return message_.c_str();}
}

