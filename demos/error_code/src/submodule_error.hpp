#pragma once

#include <system_error>

enum class SubsystemError
{
    // no 0
    InputBadAirport = 1,
    InputPastDate,
    InputBadDateRange,
    InputBadClass,
    NoFlightFound,
    NoSeatFound,
    SubsysProtocolErr,
    SubsysInternal,
    SubsysResource,
    SubsysConfig,
    SubsysTimeout,
};
 
namespace std
{
  template <>
    struct is_error_condition_enum<SubsystemError> : true_type {};
}
  
std::error_condition make_error_condition(SubsystemError e);