# An implementation of `std::thread` using WIN32 api.

 Usage example:

 ```cpp

#include "win32_thread.h"
#include <string>

int main(int argc, char **argv)
{
  // This procedure will be executed on a separate thread.
  auto your_thread_procedure = [](int32_t i32, std::string str /* and some other arguments */){
    std::cout << "I32: " << i32 << std::endl;
    std::cout << "Str: " << str << std::endl;

    // Print an id of the current thread.
    std::cout << "This thread id: " << Win32Thread::current_thread_id() << std::endl;
  };

  Win32Thread thread(your_thread_procedure, 123, "Polinka, I love you!!!");
  thread.wait_for_thread();

  return 0;
}
```
