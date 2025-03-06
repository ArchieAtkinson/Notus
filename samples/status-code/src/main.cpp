#include <zephyr/kernel.h>
#include <tl/expected.hpp>


#include "error.hpp"

template <typename ErrorEnum>
auto make_error(ErrorEnum e) {
    // Use your existing status_code helper function
    auto code = status_code(e);
    // Convert to system_code if necessary and wrap in unexpected
    return tl::make_unexpected(system_error2::system_code(code));
}

tl::expected<int, system_error2::system_code> foo(bool error)
{
  if (error)
  {
    return make_error(Error1::e1);
  }
  else 
  {
    return 5;
  }
  
}

   
int main ()
{
  auto foo_out = foo(true);
  if(!foo_out)
  {
    printk("Error %s\n", foo_out.error().message().c_str());
  }
  else
  {
    printk("Not Error %d\n", *foo_out);
  }


  return 0;
}
  
