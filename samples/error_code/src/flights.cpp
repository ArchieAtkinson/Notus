#include <cassert>

#include "submodule_error.hpp"

#include "flights.hpp"

   
namespace { // anonymous namespace
    
struct FlightsErrCategory : std::error_category
{
  const char* name() const noexcept override;
  std::string message(int ev) const override;
  std::error_condition default_error_condition(int ev) const noexcept override;
};
    
const char* FlightsErrCategory::name() const noexcept
{
  return "flights";
}
    
std::string FlightsErrCategory::message(int ev) const
{
  switch (static_cast<FlightsErrc>(ev))
  {
  case FlightsErrc::NonexistentLocations:
    return "nonexistent airport name in request";
     
  case FlightsErrc::DatesInThePast:
    return "request for a date from the past";
    
  case FlightsErrc::InvertedDates:
    return "requested flight return date before departure date";
    
  case FlightsErrc::NoFlightsFound:
    return "no filight combination found";
    
  case FlightsErrc::ProtocolViolation:
    return "received malformed request";
    
  case FlightsErrc::ConnectionError:
    return "could not connect to server";
    
  case FlightsErrc::ResourceError:
    return "insufficient resources";
    
  case FlightsErrc::Timeout:
    return "processing timed out";
    
  default:
    return "(unrecognized error)";
  }
}
 
std::error_condition FlightsErrCategory::default_error_condition(int ev) const noexcept
{
  switch (static_cast<FlightsErrc>(ev))
  {
  case FlightsErrc::NonexistentLocations:
    return SubsystemError::InputBadAirport;
   
  case FlightsErrc::DatesInThePast:
    return SubsystemError::InputPastDate;
       
  case FlightsErrc::InvertedDates:
    return SubsystemError::InputBadDateRange;
         
  case FlightsErrc::NoFlightsFound:
    return SubsystemError::NoFlightFound;
       
  case FlightsErrc::ProtocolViolation:
    return SubsystemError::SubsysProtocolErr;
       
  case FlightsErrc::ConnectionError:
    return SubsystemError::SubsysConfig;
        
  case FlightsErrc::ResourceError:
    return SubsystemError::SubsysResource;
       
  case FlightsErrc::Timeout:
    return SubsystemError::SubsysTimeout;    
 
  default:
    assert(false);
    return {};
  }
}
    
const FlightsErrCategory theFlightsErrCategory {};
    
} // anonymous namespace
   
std::error_code make_error_code(FlightsErrc e)
{
  return {static_cast<int>(e), theFlightsErrCategory};
}
 