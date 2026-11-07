#include "erl/Vehicle.h"
#include <istream>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <utility>
namespace erl {
int TransportVehicle::totalRegistered_=0;
TransportVehicle::TransportVehicle():manifestCapacity_(4){manifest_=new ReliefCargo*[manifestCapacity_]{};++totalRegistered_;}
TransportVehicle::TransportVehicle(std::string id,std::string agency,double capacity,double fuel):id_(std::move(id)),agency_(std::move(agency)),capacityKg_(capacity),fuelLitres_(fuel),manifestCapacity_(4){if(capacity<0||fuel<0)throw std::invalid_argument("Vehicle values cannot be negative");manifest_=new ReliefCargo*[manifestCapacity_]{};++totalRegistered_;}
TransportVehicle::TransportVehicle(const TransportVehicle&o):id_(o.id_),agency_(o.agency_),capacityKg_(o.capacityKg_),fuelLitres_(o.fuelLitres_),manifestWeightKg_(o.manifestWeightKg_),status_(o.status_),manifestSize_(o.manifestSize_),manifestCapacity_(o.manifestCapacity_){manifest_=new ReliefCargo*[manifestCapacity_]{};for(std::size_t i=0;i<manifestSize_;++i)manifest_[i]=new ReliefCargo(*o.manifest_[i]);++totalRegistered_;}
TransportVehicle&TransportVehicle::operator=(const TransportVehicle&o){if(this==&o)return *this;clearManifest();delete[]manifest_;id_=o.id_;agency_=o.agency_;capacityKg_=o.capacityKg_;fuelLitres_=o.fuelLitres_;manifestWeightKg_=o.manifestWeightKg_;status_=o.status_;manifestSize_=o.manifestSize_;manifestCapacity_=o.manifestCapacity_;manifest_=new ReliefCargo*[manifestCapacity_]{};for(std::size_t i=0;i<manifestSize_;++i)manifest_[i]=new ReliefCargo(*o.manifest_[i]);return *this;}
TransportVehicle::~TransportVehicle(){clearManifest();delete[]manifest_;--totalRegistered_;} void TransportVehicle::clearManifest()noexcept{if(!manifest_)return;for(std::size_t i=0;i<manifestSize_;++i)delete manifest_[i];manifestSize_=0;manifestWeightKg_=0;}
double TransportVehicle::rangeKm()const noexcept{return fuelLitres_*6.0;} double TransportVehicle::fuelRequired(double d)const noexcept{return d/6.0;}
TransportVehicle&TransportVehicle::operator+=(const ReliefCargo&c){if(!canCarry(c.totalWeightKg()))throw CargoOverweightException(manifestWeightKg_+c.totalWeightKg(),capacityKg_);if(manifestSize_==manifestCapacity_){auto cap=manifestCapacity_*2;auto**n=new ReliefCargo*[cap]{};for(std::size_t i=0;i<manifestSize_;++i)n[i]=manifest_[i];delete[]manifest_;manifest_=n;manifestCapacity_=cap;}manifest_[manifestSize_++]=new ReliefCargo(c);manifestWeightKg_+=c.totalWeightKg();return *this;}
TransportVehicle&TransportVehicle::operator-=(const ReliefCargo&c){for(std::size_t i=0;i<manifestSize_;++i)if(manifest_[i]->id()==c.id()&&manifest_[i]->quantity()>=c.quantity()){manifestWeightKg_-=c.quantity()*manifest_[i]->unitWeightKg();if(manifest_[i]->quantity()==c.quantity()){delete manifest_[i];manifest_[i]=manifest_[manifestSize_-1];manifest_[--manifestSize_]=nullptr;}else *manifest_[i]-=c.quantity();return *this;}throw std::invalid_argument("Cargo is not present in vehicle manifest");}
std::ostream&operator<<(std::ostream&o,const TransportVehicle&v){return o<<std::quoted(v.id_)<<' '<<std::quoted(v.agency_)<<' '<<v.capacityKg_<<' '<<v.fuelLitres_<<' '<<v.manifestWeightKg_;} std::istream&operator>>(std::istream&i,TransportVehicle&v){return i>>std::quoted(v.id_)>>std::quoted(v.agency_)>>v.capacityKg_>>v.fuelLitres_>>v.manifestWeightKg_;}
ReliefVehicle::ReliefVehicle(std::string id,std::string agency,double capacity,double fuel,GPSCoordinate c):TransportVehicle(std::move(id),std::move(agency),capacity,fuel),beacon_(c){}
ReliefVehicle::~ReliefVehicle() = default;
GroundConvoy::GroundConvoy(std::string a,std::string b,double c,double d,GPSCoordinate e):ReliefVehicle(std::move(a),std::move(b),c,d,e){} double GroundConvoy::rangeKm()const noexcept{return fuelLitres()*8;} double GroundConvoy::fuelRequired(double d)const noexcept{return d/8;} bool GroundConvoy::supportsCorridor(const std::string&c)const{return c=="road"||c=="highway"||c=="urban";}
AerialTransport::AerialTransport(std::string a,std::string b,double c,double d,GPSCoordinate e):ReliefVehicle(std::move(a),std::move(b),c,d,e){} double AerialTransport::rangeKm()const noexcept{return fuelLitres()*4;} double AerialTransport::fuelRequired(double d)const noexcept{return d/4;}
AmphibiousCraft::AmphibiousCraft(std::string a,std::string b,double c,double d,GPSCoordinate e):ReliefVehicle(std::move(a),std::move(b),c,d,e),FuelPoweredVehicle(),FloatCapableVehicle(){} double AmphibiousCraft::rangeKm()const noexcept{return fuelLitres()*5;} double AmphibiousCraft::fuelRequired(double d)const noexcept{return d/5;} bool AmphibiousCraft::supportsCorridor(const std::string&c)const{return c=="road"||c=="river"||c=="flooded";}
std::unique_ptr<ReliefVehicle>makeVehicle(const std::string&k,const std::string&i,const std::string&a,double c,double f,GPSCoordinate p){
    // Registration deliberately clones a concrete prototype so the TransportVehicle
    // deep-copy constructor and heap-backed manifest ownership are exercised in the
    // normal application path.
    if(k=="Aerial"){AerialTransport prototype(i,a,c,f,p);std::stringstream encoded;encoded<<static_cast<const TransportVehicle&>(prototype);TransportVehicle parsed;encoded>>parsed;if(parsed.id()!=i||parsed.capacityKg()!=c||parsed.fuelLitres()!=f)return nullptr;return std::make_unique<AerialTransport>(prototype);}
    if(k=="Amphibious"){AmphibiousCraft prototype(i,a,c,f,p);std::stringstream encoded;encoded<<static_cast<const TransportVehicle&>(prototype);TransportVehicle parsed;encoded>>parsed;if(parsed.id()!=i||parsed.capacityKg()!=c||parsed.fuelLitres()!=f)return nullptr;return std::make_unique<AmphibiousCraft>(prototype);}
    GroundConvoy prototype(i,a,c,f,p);std::stringstream encoded;encoded<<static_cast<const TransportVehicle&>(prototype);TransportVehicle parsed;encoded>>parsed;if(parsed.id()!=i||parsed.capacityKg()!=c||parsed.fuelLitres()!=f)return nullptr;return std::make_unique<GroundConvoy>(prototype);
}
}

