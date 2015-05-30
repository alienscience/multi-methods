
# Super simple multimethods in C++

A more efficient implementation of multimethods can be found at:
   [https://github.com/jll63/yomm11](https://github.com/jll63/yomm11)

The implementation here has the following disadvantages:
* Slow, O(N) virtual function calls and copy constructor calls where N is the number of arguments.
* Limited to a maximum 3 arguments.
* Generates O(N^2) classes where N is the number of argument types (-O2).

And the following advantages:
* Simple, header only (<250 lines).
* Macro free.
* Does not require a class hierarchy.

## Example

```c++

#include "multimethods.h"

struct DumpArgs {

    int result;

    // Multimethod implementation
    template <typename T0, typename T1, typename T2>
    void apply(const T0& arg0, const T1& arg1, const T2& arg2) {
        std::cout << arg0 << " "
                  << arg1 << " "
                  << arg2 << std::endl;
        result = arg0 + arg1 + arg2;
    }
};

// Wrap a DumpArgs object as multimethod that can use int or double
DumpArgs dumpArgs;
auto method = multi::method<int,double>(dumpArgs);

// Add arguments
method.addArg(0.1);
method.addArg(200);

// Call method
method.apply();

// Get results
std::cout << "Result: " << dumpArgs.result << std::endl;
```

Also see main.cc

## Requirements

Requires C++11.

## Licence

MIT

