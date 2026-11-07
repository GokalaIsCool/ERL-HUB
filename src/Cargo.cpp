#include "erl/Cargo.h"
#include <istream>
#include <ostream>
#include <stdexcept>
#include <utility>
namespace erl {
ReliefCargo::ReliefCargo(std::string id,std::string category,int quantity,double unitWeightKg,int priority):id_(std::move(id)),category_(std::move(category)),quantity_(quantity),unitWeightKg_(unitWeightKg),priority_(priority){if(quantity<0||unitWeightKg<0)throw std::invalid_argument("Cargo values cannot be negative");}
ReliefCargo& ReliefCargo::operator+=(int q){if(q<0)throw std::invalid_argument("Cargo increment cannot be negative");quantity_+=q;return *this;}
ReliefCargo& ReliefCargo::operator-=(int q){if(q<0||q>quantity_)throw std::invalid_argument("Cargo subtraction exceeds stock");quantity_-=q;return *this;}
bool ReliefCargo::operator==(const ReliefCargo&o)const noexcept{return id_==o.id_&&category_==o.category_&&quantity_==o.quantity_&&unitWeightKg_==o.unitWeightKg_&&priority_==o.priority_;}
std::ostream& operator<<(std::ostream&out,const ReliefCargo&c){return out<<c.id_<<' '<<c.category_<<' '<<c.quantity_<<' '<<c.unitWeightKg_<<' '<<c.priority_;}
std::istream& operator>>(std::istream&in,ReliefCargo&c){return in>>c.id_>>c.category_>>c.quantity_>>c.unitWeightKg_>>c.priority_;}
}

