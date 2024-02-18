#include "submodule_error.hpp"
#include "queries.hpp"
  
namespace {
 
class SubsystemErrorCategory : public std::error_category
{
public:
  const char* name() const noexcept override;
  std::string message(int ev) const override;
};
     
const char* SubsystemErrorCategory::name() const noexcept
{
    return "submodule-error";
}
      
std::string SubsystemErrorCategory::message(int ev) const
{
  switch (static_cast<SubsystemError>(ev))
  {
      default:
          return "(uncharted)";
  }
}
 
const SubsystemErrorCategory theSubsystemErrorCategory {};
 
 
}
 
 
std::error_condition make_error_condition(SubsystemError e)
{
  return {static_cast<int>(e), theSubsystemErrorCategory};
}
 
// -----------------
 
namespace {
class FailureSourceCategory : public std::error_category
{
public:
  const char* name() const noexcept override;
  std::string message(int ev) const override;
  bool equivalent(
      const std::error_code& code,
      int condition) const noexcept override;
};
      
const char* FailureSourceCategory::name() const noexcept
{
    return "failure-source";
}
      
std::string FailureSourceCategory::message(int ev) const
{
  switch (static_cast<FailureSource>(ev))
  {
  case FailureSource::BadUserInput:
    return "invalid user request";
     
  case FailureSource::SystemError:
    return "internal error";
    
  case FailureSource::NoSolution:
    return "no solution found for specified request";
    
  default:
    return "(unrecognized condition)";
  }
}
      
bool FailureSourceCategory::equivalent(
      const std::error_code& ec,
      int cond) const noexcept
{     
  switch (static_cast<FailureSource>(cond))
  {
  case FailureSource::BadUserInput:
    return ec == SubsystemError::InputBadAirport
        || ec == SubsystemError::InputPastDate
        || ec == SubsystemError::InputBadDateRange
        || ec == SubsystemError::InputBadClass;
 
  case FailureSource::SystemError:
    return ec == SubsystemError::SubsysProtocolErr
        || ec == SubsystemError::SubsysInternal
        || ec == SubsystemError::SubsysResource
        || ec == SubsystemError::SubsysConfig        
        || ec == SubsystemError::SubsysTimeout;
         
  case FailureSource::NoSolution:
    return ec == SubsystemError::NoFlightFound
        || ec == SubsystemError::NoSeatFound;
              
  default:
    return false;
  }
}
      
const FailureSourceCategory theFailureSourceCategory {};
    
} // anonymous namespace
   
std::error_condition make_error_condition(FailureSource e)
{
  return {static_cast<int>(e), theFailureSourceCategory};
}
 
// --------------
namespace {
class SeverityCategory : public std::error_category
{
public:
  const char* name() const noexcept override;
  std::string message(int ev) const override;
  bool equivalent(
      const std::error_code& code,
      int condition) const noexcept override;
};
      
const char* SeverityCategory::name() const noexcept
{
    return "severity";
}
      
std::string SeverityCategory::message(int ev) const
{
  switch (static_cast<Severity>(ev))
  {
  case Severity::Bug:
    return "failure is a programmer bug";
     
  case Severity::Config:
    return "failure is a configuration error";
     
  case Severity::Resource:
    return "failure is caused by resource shortage";
   
  case Severity::Normal:
    return "failure requires no attention";  
   
  default:
    return "(unrecognized condition)";
  }
}
      
bool SeverityCategory::equivalent(
      const std::error_code& ec,
      int cond) const noexcept
{     
  switch (static_cast<Severity>(cond))
  {
  case Severity::Bug:
    return ec == SubsystemError::SubsysProtocolErr
        || ec == SubsystemError::SubsysInternal;
         
  case Severity::Config:
    return ec == SubsystemError::SubsysConfig;
 
  case Severity::Resource:
    return ec == SubsystemError::SubsysResource        
        || ec == SubsystemError::SubsysTimeout;
         
  case Severity::Normal:
    return ec == SubsystemError::InputBadAirport
        || ec == SubsystemError::InputPastDate
        || ec == SubsystemError::InputBadDateRange
        || ec == SubsystemError::InputBadClass
        || ec == SubsystemError::NoFlightFound
        || ec == SubsystemError::NoSeatFound;
              
  default:
    return false;
  }
}
      
const SeverityCategory theSeverityCategory {};
    
} // anonymous namespace
   
std::error_condition make_error_condition(Severity e)
{
  return {static_cast<int>(e), theSeverityCategory};
}