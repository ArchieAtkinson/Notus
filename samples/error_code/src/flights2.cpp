#include <cassert>

#include "submodule_error.hpp"

#include "flights2.hpp"
   
namespace { // anonymous namespace
    
struct Flights2ErrCategory : std::error_category
{
  const char* name() const noexcept override;
  std::string message(int ev) const override;
  std::error_condition default_error_condition(int ev) const noexcept override;
};
    
const char* Flights2ErrCategory::name() const noexcept
{
  return "flights2";
}
    
std::string Flights2ErrCategory::message(int ev) const
{
  switch (static_cast<Flights2Errc>(ev))
  {
  // return name for each enum
  default:
    return "(unrecognized error)";
  }
}
 
std::error_condition Flights2ErrCategory::default_error_condition(int ev) const noexcept
{
  switch (static_cast<Flights2Errc>(ev))
  {
  case Flights2Errc::ZeroFlightsFound:
    return SubsystemError::NoFlightFound;
     
  case Flights2Errc::InvalidRequestFormat:
    return SubsystemError::SubsysProtocolErr;
       
  case Flights2Errc::CouldNotConnect:
    return SubsystemError::SubsysConfig;
       
  case Flights2Errc::DatabaseError:
    return SubsystemError::SubsysInternal;
       
  case Flights2Errc::NotEnoughMemory:
    return SubsystemError::SubsysResource;
       
  case Flights2Errc::InternalError:
    return SubsystemError::SubsysInternal;
       
  case Flights2Errc::NoSuchAirport:
    return SubsystemError::InputBadAirport;
       
  case Flights2Errc::JourneyInThePast:
    return SubsystemError::InputPastDate;
       
  case Flights2Errc::DatesNotMonotonic:
    return SubsystemError::InputBadDateRange;
     
  default:
    assert(false);
    return {};
  }
}
    
const Flights2ErrCategory theFlights2ErrCategory {};
    
} // anonymous namespace
   
std::error_code make_error_code(Flights2Errc e)
{
  return {static_cast<int>(e), theFlights2ErrCategory};
}
