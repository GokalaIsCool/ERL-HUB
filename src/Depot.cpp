#include "erl/Depot.h"
#include "erl/Exceptions.h"
#include "erl/Utilities.h"
#include <istream>
#include <iomanip>
#include <cmath>
#include <ostream>
#include <stdexcept>
#include <utility>
namespace erl {
double DistributionDepot::totalCapacityKg_=0.0;
DistributionDepot::DistributionDepot(std::string id,std::string name,double capacity):id_(std::move(id)),name_(std::move(name)),capacityKg_(capacity){if(id_.empty()||name_.empty()||!std::isfinite(capacity)||capacity<0)throw std::invalid_argument("Invalid depot values");totalCapacityKg_+=capacityKg_;}
DistributionDepot::DistributionDepot(const DistributionDepot&o):id_(o.id_),name_(o.name_),capacityKg_(o.capacityKg_),currentWeightKg_(o.currentWeightKg_),inventory_(o.inventory_){totalCapacityKg_+=capacityKg_;}
DistributionDepot::DistributionDepot(DistributionDepot&&o)noexcept:id_(std::move(o.id_)),name_(std::move(o.name_)),capacityKg_(o.capacityKg_),currentWeightKg_(o.currentWeightKg_),inventory_(std::move(o.inventory_)){o.capacityKg_=0;o.currentWeightKg_=0;}
DistributionDepot& DistributionDepot::operator=(const DistributionDepot&o){if(this==&o)return *this;DistributionDepot copy(o);return *this=std::move(copy);}
DistributionDepot& DistributionDepot::operator=(DistributionDepot&&o)noexcept{if(this==&o)return *this;totalCapacityKg_+=o.capacityKg_-capacityKg_;id_=std::move(o.id_);name_=std::move(o.name_);capacityKg_=o.capacityKg_;currentWeightKg_=o.currentWeightKg_;inventory_=std::move(o.inventory_);o.capacityKg_=0;o.currentWeightKg_=0;return *this;}
DistributionDepot::~DistributionDepot(){totalCapacityKg_-=capacityKg_;}
bool DistributionDepot::addCargo(const ReliefCargo&c){if(currentWeightKg_+c.totalWeightKg()>capacityKg_)throw CargoOverweightException(currentWeightKg_+c.totalWeightKg(),capacityKg_);auto it=inventory_.find(c.id());if(it==inventory_.end())inventory_.emplace(c.id(),c);else if(it->second==c)it->second+=c.quantity();else{if(it->second.category()!=c.category()||it->second.unitWeightKg()!=c.unitWeightKg()||it->second.priority()!=c.priority())throw std::invalid_argument("Cargo metadata mismatch");it->second+=c.quantity();}currentWeightKg_+=c.totalWeightKg();return true;}
bool DistributionDepot::removeCargo(const std::string&id,int q){auto it=inventory_.find(id);if(it==inventory_.end()||!ReliefOps::Inventory::canAllocate(it->second.quantity(),q))return false;currentWeightKg_-=q*it->second.unitWeightKg();it->second-=q;if(it->second.quantity()==0)inventory_.erase(it);return true;}
ReliefCargo* DistributionDepot::cargo(const std::string&id){auto it=inventory_.find(id);return it==inventory_.end()?nullptr:&it->second;} const ReliefCargo* DistributionDepot::cargo(const std::string&id)const{auto it=inventory_.find(id);return it==inventory_.end()?nullptr:&it->second;}
std::vector<ReliefCargo> DistributionDepot::inventory()const{std::vector<ReliefCargo>r;for(const auto&e:inventory_)r.push_back(e.second);return r;}
DistributionDepot& DistributionDepot::operator+=(const ReliefCargo&c){addCargo(c);return *this;} DistributionDepot& DistributionDepot::operator-=(const ReliefCargo&c){if(!removeCargo(c.id(),c.quantity()))throw std::invalid_argument("Cargo is not available in depot");return *this;}
std::ostream&operator<<(std::ostream&o,const DistributionDepot&d){return o<<std::quoted(d.id_)<<' '<<std::quoted(d.name_)<<' '<<d.capacityKg_<<' '<<d.currentWeightKg_;} std::istream&operator>>(std::istream&i,DistributionDepot&d){std::string id,name;double capacity=0.0,current=0.0;if(!(i>>std::quoted(id)>>std::quoted(name)>>capacity>>current))return i;if(id.empty()||name.empty()||!std::isfinite(capacity)||capacity<0||!std::isfinite(current)||current<0||current>capacity){i.setstate(std::ios::failbit);return i;}DistributionDepot parsed(std::move(id),std::move(name),capacity);parsed.currentWeightKg_=current;d=std::move(parsed);return i;}
}
namespace ReliefOps::Inventory {
bool allocateSupply(erl::DistributionDepot&d,const std::string&id,int q,erl::ReliefCargo&a){const auto*s=d.cargo(id);if(!s||!canAllocate(s->quantity(),q))return false;a=erl::ReliefCargo(s->id(),s->category(),q,s->unitWeightKg(),s->priority());d-=a;return true;}
bool allocateSupply(erl::DistributionDepot&d,erl::ReliefCargo&s,int q,erl::ReliefCargo&a){if(!canAllocate(s.quantity(),q))return false;const auto id=s.id();a=erl::ReliefCargo(s.id(),s.category(),q,s.unitWeightKg(),s.priority());s-=q;if(d.cargo(id))d-=a;return true;}
}

