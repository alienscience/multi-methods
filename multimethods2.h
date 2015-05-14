#ifndef MULITMETHODS2_H
#define MULITMETHODS2_H

namespace multi {

// Create an interface based on the given parameter pack

template <typename... Ts> struct ArgList {};

template <typename T, typename... Ts>
struct ArgList<T, Ts...> : public ArgList<Ts...>
{
    virtual ArgList* addArg(T& v) = 0;
};

// Create a base class the implements the interface

template <typename... TArgs, typename... Ts>
struct Builder : public ArgList<TArgs...> {};

template <typename T, typename... Ts>
struct Builder<T, Ts...> : public Builder<Ts...>
{
    virtual ArgList<Ts...>* addArg(T& v);
};

// Create a class that sets up the builder

// First argument

template <typename T0, typename... Ts> struct Arg0 : public Builder<Ts...>
{
protected:
    T0& arg0_;
public:
    Arg0(T0& v) : arg0_(v) {}
};

template <typename T0, typename T, typename... Ts>
struct Arg0<T0, T, Ts...> : public Arg0<T0, Ts...> {
public:
    virtual ArgList<Ts...>* addArg(T& v) { return this; }
};

//----- Template method implementations ----------------------------------------

template <typename T, typename... Ts>
ArgList<Ts...>* Builder<T, Ts...>::addArg(T& v)
{
    return new Arg0<T, Ts...>(v);
}

} // namespace multi
#endif // MULITMETHODS2_H

