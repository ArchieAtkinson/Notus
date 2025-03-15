#include <zephyr/kernel.h>

#include "error.hpp"
#include "outcome.hpp"

namespace outcome = OUTCOME_V2_NAMESPACE;

outcome::result<int> foo(bool error)
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

outcome::result<int> bar()
{
  return OUTCOME_TRYX(foo(true));
}
   
int main ()
{
  auto foo_out = bar();
  if(!foo_out)
  {
    printk("Error %d\n", foo_out.error());
  }
  else
  {
    printk("Not Error %d\n", foo_out.value());
  }


  return 0;
}
  
