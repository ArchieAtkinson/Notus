#pragma once

#include <system_error>
   
enum class Flights2Errc
{
  ZeroFlightsFound     = 11,
  InvalidRequestFormat = 12,
  CouldNotConnect      = 13,
  DatabaseError        = 14,
  NotEnoughMemory      = 15,
  InternalError        = 16,
  NoSuchAirport        = 17,
  JourneyInThePast     = 18,
  DatesNotMonotonic    = 19,
};
   
namespace std
{
  template <>
    struct is_error_code_enum<Flights2Errc> : true_type {};
}
   
std::error_code make_error_code(Flights2Errc);