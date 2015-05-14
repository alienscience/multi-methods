#include <iostream>
//#include "multimethods.h"
#include "multimethods2.h"


struct DumpArgs {

    // Multimethod definition
    template <typename T0, typename T1, typename T2>
    void apply(T0& arg0, T1& arg1, T2& arg2) {
        std::cout << arg0 << " "
                  << arg1 << " "
                  << arg2 << std::endl;
    }
};

int main()
{
    multi::Method<DumpArgs,int,double> method;
    //multi::Builder<int,double> builder;
#if 0
    multi::Builder<DumpArgs> builder;
    int arg1 = 1;
    double arg2 = 2.1;
    int arg3 = 3;
    multi::ArgList* args = builder.addArg(arg1);
    args->apply();
    args = args->addArg(arg2);
    args->apply();
    args = args->addArg(arg3);
    args->apply();
#endif
    return 0;
}

