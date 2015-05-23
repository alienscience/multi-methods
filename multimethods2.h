#ifndef MULITMETHODS2_H
#define MULITMETHODS2_H

namespace multi {

// Builder is a container template that holds all the possible types
// of multimethod arguments
template <typename... TArgs>
struct Builder
{
    // Create an interface based on the given parameter pack
    template <typename... Ts> struct ArgList {};

    template <typename T, typename... Ts>
    struct ArgList<T, Ts...> : public ArgList<Ts...>
    {
        virtual ArgList<TArgs...>* addArg(T& v) = 0;
    };

    //-------------------------------------------------------------------------

    template <typename... Ts> struct ArgStart : public ArgList<TArgs...> {};

    template <typename T, typename... Ts>
    struct ArgStart<T, Ts...> : public ArgStart<Ts...>
    {
        virtual ArgList<TArgs...>* addArg(T& v);
    };

    //-------------------------------------------------------------------------

    template <typename T0, typename... Ts>
    struct Arg0 : public ArgList<TArgs...>
    {
        T0& arg0_;
        Arg0(T0& v) : arg0_(v) {}
    };

    template <typename T0, typename T, typename... Ts>
    struct Arg0<T0, T, Ts...> : public Arg0<T0,Ts...>
    {
        Arg0<T0,T,Ts...>(T0& v) : Arg0<T0,Ts...>(v) {}
        virtual ArgList<TArgs...>* addArg(T& v);
    };

    //-------------------------------------------------------------------------

    template <typename T0, typename T1, typename... Ts>
    struct Arg1 : public Arg0<T0,Ts...>
    {
        T1& arg1_;
        Arg1(T0& arg0, T1& v) : Arg0<T0,Ts...>(arg0), arg1_(v) {}
    };

    template <typename T0, typename T1, typename T, typename... Ts>
    struct Arg1<T0, T1, T, Ts...> : public Arg1<T0,T1,Ts...>
    {
        Arg1<T0,T1,T,Ts...>(T0& arg0, T1& v) : Arg1<T0,T1,Ts...>(arg0,v) {}
        virtual ArgList<TArgs...>* addArg(T& v);
    };

    //-------------------------------------------------------------------------

    template <typename T0, typename T1, typename T2, typename... Ts>
    struct Arg2 : public Arg1<T0,T1,Ts...>
    {
        T2& arg2_;
        Arg2(T0& arg0, T1& arg1, T2& v) :
            Arg1<T0,T1,Ts...>(arg0, arg1),
            arg2_(v) {}
    };

    template <typename T0, typename T1, typename T2, typename T, typename... Ts>
    struct Arg2<T0, T1, T2, T, Ts...> : public Arg2<T0,T1,T2,Ts...>
    {
        Arg2<T0,T1,T2,T,Ts...>(T0& arg0, T1& arg1, T2& v) :
            Arg2<T0,T1,T2,Ts...>(arg0,arg1,v) {}
        virtual ArgList<TArgs...>* addArg(T& v) { return this; }
    };


};


//----- Template method implementations ----------------------------------------

template <typename... TArgs>
template <typename T, typename... Ts>
Builder<TArgs...>::ArgList<TArgs...>*
Builder<TArgs...>::ArgStart<T,Ts...>::addArg(T& v)
{
    return new Arg0<T,TArgs...>(v);
}

template <typename... TArgs>
template <typename T0, typename T, typename... Ts>
Builder<TArgs...>::ArgList<TArgs...>*
Builder<TArgs...>::Arg0<T0,T,Ts...>::addArg(T& v)
{
    return new Arg1<T0,T,TArgs...>(Arg0<T0>::arg0_,v);
}

template <typename... TArgs>
template <typename T0, typename T1, typename T, typename... Ts>
Builder<TArgs...>::ArgList<TArgs...>*
Builder<TArgs...>::Arg1<T0,T1,T,Ts...>::addArg(T& v)
{
    return new Arg2<T0,T1,T,TArgs...>(Arg0<T0>::arg0_,Arg1<T0,T1>::arg1_,v);
}

} // namespace multi
#endif // MULITMETHODS2_H

