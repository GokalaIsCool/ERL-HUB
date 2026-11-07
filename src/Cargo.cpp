#include "erl/Cargo.h"
#include <istream>
#include <iomanip>
#include <limits>
#include <cmath>
#include <ostream>
#include <stdexcept>
#include <utility>
namespace erl {
ReliefCargo::ReliefCargo(std::string id,std::string category,int quantity,double unitWeightKg,int priority):id_(std::move(id)),category_(std::move(category)),quantity_(quantity),unitWeightKg_(unitWeightKg),priority_(priority){if(id_.empty()||category_.empty()||quantity<0||!std::isfinite(unitWeightKg)||unitWeightKg<0||priority<1||priority>100)throw std::invalid_argument("Invalid cargo values");}
ReliefCargo::~ReliefCargo() = default;
ReliefCargo& ReliefCargo::operator+=(int q){if(q<0||q>std::numeric_limits<int>::max()-quantity_)throw std::invalid_argument("Cargo increment is invalid");quantity_+=q;return *this;}
ReliefCargo& ReliefCargo::operator-=(int q){if(q<0||q>quantity_)throw std::invalid_argument("Cargo subtraction exceeds stock");quantity_-=q;return *this;}
bool ReliefCargo::operator==(const ReliefCargo&o)const noexcept{return id_==o.id_&&category_==o.category_&&quantity_==o.quantity_&&unitWeightKg_==o.unitWeightKg_&&priority_==o.priority_;}
std::ostream& operator<<(std::ostream&out,const ReliefCargo&c){return out<<std::quoted(c.id_)<<' '<<std::quoted(c.category_)<<' '<<c.quantity_<<' '<<c.unitWeightKg_<<' '<<c.priority_;}
std::istream& operator>>(std::istream&in,ReliefCargo&c){std::string id,category;int quantity=0,priority=0;double unitWeight=0.0;if(!(in>>std::quoted(id)>>std::quoted(category)>>quantity>>unitWeight>>priority))return in;if(id.empty()||category.empty()||quantity<0||!std::isfinite(unitWeight)||unitWeight<0||priority<1||priority>100){in.setstate(std::ios::failbit);return in;}c.id_=std::move(id);c.category_=std::move(category);c.quantity_=quantity;c.unitWeightKg_=unitWeight;c.priority_=priority;return in;}
}

