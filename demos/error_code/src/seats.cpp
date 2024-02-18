#include <cassert>

#include "submodule_error.hpp"
#include "seats.hpp"
   
namespace { // anonymous namespace
    
struct SeatsErrCategory : std::error_category
{
  const char* name() const noexcept override;
  std::string message(int ev) const override;
  std::error_condition default_error_condition(int ev) const noexcept override;
};
    
const char* SeatsErrCategory::name() const noexcept
{
  return "seats";
}
    
std::string SeatsErrCategory::message(int ev) const
{
  switch (static_cast<SeatsErrc>(ev))
  {      
  case SeatsErrc::InvalidRequest:
    return "received malformed request";
   
  case SeatsErrc::CouldNotConnect:
    return "could not connect to server";
      
  case SeatsErrc::InternalError:
    return "insufficient resources";
      
  case SeatsErrc::NoResponse:
    return "processing timed out";
            
  case SeatsErrc::NonexistentClass:
    return "requested class does not exist";
     
  case SeatsErrc::NoSeatAvailable:
    return "all seats booked";
    
  default:
    return "(unrecognized error)";
  }
}
 
std::error_condition SeatsErrCategory::default_error_condition(int ev) const noexcept
{
  switch (static_cast<SeatsErrc>(ev))
  {
    case SeatsErrc::InvalidRequest:
      return SubsystemError::SubsysProtocolErr;
     
    case SeatsErrc::CouldNotConnect:
      return SubsystemError::SubsysConfig;
       
    case SeatsErrc::InternalError:
      return SubsystemError::SubsysInternal;
       
    case SeatsErrc::NoResponse:
      return SubsystemError::SubsysTimeout;
       
    case SeatsErrc::NonexistentClass:
      return SubsystemError::InputBadClass;
       
    case SeatsErrc::NoSeatAvailable:
      return SubsystemError::NoSeatFound;
     
    default:
      assert(false);
      return {};
  }
}
    
const SeatsErrCategory theSeatsErrCategory {};
    
} // anonymous namespace
   
std::error_code make_error_code(SeatsErrc e)
{
  return {static_cast<int>(e), theSeatsErrCategory};
}