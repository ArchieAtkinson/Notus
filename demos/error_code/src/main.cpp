#include <cstring>
#include <iterator>
#include <zephyr/kernel.h>

#include "flights.hpp"
#include "seats.hpp"
#include "submodule_error.hpp"
#include "queries.hpp"

#include "magic_enum.hpp"

#include <cassert>
#include <iostream>
#include <map>
#include <utility>

std::map<std::pair<std::string, int>, std::string> my_map = 
{
    {std::make_pair("flights", 20), "NoFlightsFound"}
};

   
int main ()
{
  std::error_code ec = FlightsErrc::NoFlightsFound;   
  assert (ec == SubsystemError::NoFlightFound);
  assert (ec != SubsystemError::InputBadClass);
  assert (ec == FailureSource::NoSolution);
  assert (ec != FailureSource::BadUserInput);
  assert (ec == Severity::Normal);
  assert(ec != Severity::Bug);

  std::cout << my_map.at(std::make_pair(ec.category().name(), ec.value())) << std::endl;
      
  ec = SeatsErrc::NonexistentClass;
  assert (ec != SubsystemError::NoFlightFound);
  assert (ec == SubsystemError::InputBadClass);
  assert (ec != FailureSource::NoSolution);
  assert (ec == FailureSource::BadUserInput);
  assert (ec == Severity::Normal);
  assert (ec != Severity::Config);

  std::cout << ec.category().name() << std::endl;
  std::cout << ec.message() << std::endl;
  std::cout << ec.value() << std::endl;
  
 
  // the following illustrates how you insert
  //   raw int values into error_code system:
  int ret_value = 20; // it was returned from Flights
  std::error_code ec2 = static_cast<FlightsErrc>(ret_value);
  std::cout << ec2 << std::endl;
}
  