#include <iostream>
//#include "multimethods.h"
#include "multimethods2.h"


struct DumpArgs {

    // multimethod definition
    template <typename T0, typename T1, typename T2>
    void apply(T0& arg0, T1& arg1, T2& arg2) {
        std::cout << arg0 << " "
                  << arg1 << " "
                  << arg2 << std::endl;
    }
};

int main()
{
    auto method = multi::method<DumpArgs,int,double>();
    int arg1 = 1;
    double arg2 = 2.1;
    int arg3 = 3;
    method.addArg(arg1);
    method.addArg(arg2);
    method.addArg(arg3);
    method.apply();
    return 0;
}

