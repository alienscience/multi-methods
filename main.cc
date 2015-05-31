#include <iostream>
#include "multimethods.h"

#include <random>
#include <chrono>

struct DumpArgs {

    int result;
    DumpArgs() : result(-1) {}
    virtual ~DumpArgs() {}

    // Multimethod definition
    template <typename T0, typename T1, typename T2>
    void apply(const T0& arg0, const T1& arg1, const T2& arg2) {
        std::cout << arg0 << " "
                  << arg1 << " "
                  << arg2 << std::endl;
        result = arg0 + arg1 + arg2;
    }
};

int main()
{
    // Example of multimethod use

    // Setup random number generation
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distribution(0,100);

    // Define a multimethod that accepts arguments of int or double
    DumpArgs dumpArgs;
    auto method = multi::method<int,double>(dumpArgs);

    // Add arguments of types randomly selected at runtime
    int rnd;
    for (int i = 0; i < 3; i++) {
        rnd = distribution(generator);
        if (rnd < 50) {
            // Integer argument
            method.visit(rnd);
        } else {
            // Double argument
            double dbl = static_cast<double>(rnd) + 0.01;
            method.visit(dbl);
        }
    }

    // Call the appropriate version of DumpArgs::apply()
    method.apply();

    // Results can be extracted from DumpArgs
    std::cout << "Integer summation " << dumpArgs.result << std::endl;

    return 0;
}

