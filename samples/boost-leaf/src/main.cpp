#include "zephyr/fatal_types.h"
#include <zephyr/kernel.h>


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "../leaf.hpp"
#pragma GCC diagnostic pop

namespace leaf = boost::leaf;


#define BOOST_NO_EXCEPTIONS
void boost::throw_exception(std::exception const & e)
{
  ARG_UNUSED(e);
  k_oops();
  while(true);
}

enum class Error1
{
  e1,
  e2,
  e3
};

enum class Error2
{
  e1,
  e2,
  e3
};

leaf::result<int> foo(bool error)
{
  if (error)
  {
    return leaf::new_error(Error1::e1);
  }
  else 
  {
    return 5;
  }
  
}

leaf::result<int> bar(bool error)
{
  if (error)
  {
    return leaf::new_error(Error2::e1);
  }
  else 
  {
    return 6;
  }
  
}
   
int main ()
{
  leaf::result<int> r = leaf::try_handle_some(
     []() -> leaf::result<int>
     {
       int out {};
       BOOST_LEAF_ASSIGN(out, foo(false));
       BOOST_LEAF_ASSIGN(out, bar(true));

       return out;
     },
     [](Error1 error) -> leaf::result<int> 
     {
       if (error == Error1::e1)
       {
       printk("E1 err1\n");
         
       }
       else {
         printk("Unknown Err\n");
       }
       return 0;
     },
     [](Error2 error) -> leaf::result<int> 
     {
       if (error == Error2::e1)
       {
       printk("E2 err1\n");
         
       }
       else {
         printk("Unknown Err\n");
       }
       return 0;
     }
    
  );

  printk("Num: %d\n", r.value());
  

  return 0;
}
  
