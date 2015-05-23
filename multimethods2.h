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

    // An empty ArgList
    template <typename... Ts> struct ArgStart : public ArgList<TArgs...> {};

    template <typename T, typename... Ts>
    struct ArgStart<T, Ts...> : public ArgStart<Ts...>
    {
        virtual ArgList<TArgs...>* addArg(T& v);
    };

    // A one argument ArgList
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
        virtual ArgList<TArgs...>* addArg(T& v) { return this; }
    };

};


//----- Template method implementations ----------------------------------------

template <typename... TArgs>
template <typename T, typename... Ts>
Builder<TArgs...>::ArgList<TArgs...>*
Builder<TArgs...>::ArgStart<T, Ts...>::addArg(T& v)
{
    return new Arg0<T,TArgs...>(v);
}

} // namespace multi
#endif // MULITMETHODS2_H

