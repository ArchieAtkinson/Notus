#include <zephyr/kernel.h>

#include "error.hpp"
#include "outcome-experimental.hpp"

namespace outcome = OUTCOME_V2_NAMESPACE::experimental;

outcome::status_result<int> foo(bool error)
{
  if (error)
  {
    return Error1::e1;
  }
  else 
  {
    return 5;
  }
  
}

outcome::status_result<int> bar()
{
  return OUTCOME_TRYX(foo(true));
}
   
int main ()
{
  auto foo_out = bar();
  if(!foo_out)
  {
    printk("Error %d\n", foo_out.error().value());
  }
  else
  {
    printk("Not Error %d\n", foo_out.value());
  }


  return 0;
}
  
