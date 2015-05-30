#ifndef MULITMETHODS2_H
#define MULITMETHODS2_H

namespace multi {

// Builder is a container template that holds all the possible types
// of multimethod arguments
template <typename... TArgs>
struct Builder
{
    // Create an interface based on the given parameter pack
    template <typename... Ts> struct ArgList
    {
    };

    template <typename T>
    struct ArgList<T>
    {
        virtual ArgList<TArgs...>* addArg(T& v) = 0;
    };

    template <typename T, typename... Ts>
    struct ArgList<T, Ts...> : public ArgList<Ts...>
    {
        // Premote functions from the base classes
        using ArgList<Ts...>::addArg;
        virtual ArgList<TArgs...>* addArg(T& v) = 0;
        virtual void apply() = 0;
    };

    //-------------------------------------------------------------------------

    template <size_t k, typename TImpl,typename... Ts> struct ArgStart :
            public ArgList<TArgs...>,
            public TImpl
    {
    };

    template <size_t k, typename TImpl, typename T, typename... Ts>
    struct ArgStart<k, TImpl, T, Ts...> : public ArgStart<k,TImpl,Ts...>
    {
        using ArgStart<k,TImpl,Ts...>::addArg;
        virtual ArgList<TArgs...>* addArg(T& v);
        virtual void apply(){}
    };

    //-------------------------------------------------------------------------

    template <size_t k, typename TImpl, typename T0, typename... Ts>
    struct Arg0 : public ArgList<TArgs...>
    {
        T0& arg0_;
        Arg0(T0& v) : arg0_(v) {}
    };

    template <size_t k, typename TImpl, typename T0, typename T, typename... Ts>
    struct Arg0<k, TImpl, T0, T, Ts...> : public Arg0<k,TImpl,T0,Ts...>
    {
        Arg0<k,TImpl,T0,T,Ts...>(T0& v) : Arg0<k,TImpl,T0,Ts...>(v) {}
        using Arg0<k,TImpl,T0,Ts...>::addArg;
        virtual ArgList<TArgs...>* addArg(T& v);
        virtual void apply(){}
    };

    //-------------------------------------------------------------------------

    template <size_t k, typename TImpl, typename T0, typename T1, typename... Ts>
    struct Arg1 : public Arg0<k,TImpl,T0,Ts...>
    {
        T1& arg1_;
        Arg1(T0& arg0, T1& v) : Arg0<k,TImpl,T0,Ts...>(arg0), arg1_(v) {}
    };

    template <size_t k, typename TImpl, typename T0, typename T1, typename T, typename... Ts>
    struct Arg1<k,TImpl,T0, T1, T, Ts...> : public Arg1<k,TImpl,T0,T1,Ts...>
    {
        Arg1<k,TImpl,T0,T1,T,Ts...>(T0& arg0, T1& v) : Arg1<k,TImpl,T0,T1,Ts...>(arg0,v) {}
        using Arg1<k,TImpl,T0,T1,Ts...>::addArg;
        virtual ArgList<TArgs...>* addArg(T& v);
        virtual void apply()
        {
            TImpl impl;
            impl.apply(Arg0<k,TImpl,T0>::arg0_, Arg1<k,TImpl,T0,T1>::arg1_);
        }
    };

    //-------------------------------------------------------------------------

    template <size_t k, typename TImpl,typename T0, typename T1, typename T2, typename... Ts>
    struct Arg2 : public Arg1<k,TImpl,T0,T1,Ts...>
    {
        T2& arg2_;
        Arg2(T0& arg0, T1& arg1, T2& v) :
            Arg1<k,TImpl,T0,T1,Ts...>(arg0, arg1),
            arg2_(v) {}
    };

    template <size_t k, typename TImpl,typename T0, typename T1, typename T2, typename T, typename... Ts>
    struct Arg2<k,TImpl,T0, T1, T2, T, Ts...> : public Arg2<k,TImpl,T0,T1,T2,Ts...>
    {
        Arg2<k,TImpl,T0,T1,T2,T,Ts...>(T0& arg0, T1& arg1, T2& v) :
            Arg2<k,TImpl,T0,T1,T2,Ts...>(arg0,arg1,v) {}
        using Arg2<k,TImpl,T0,T1,T2,Ts...>::addArg;
        virtual ArgList<TArgs...>* addArg(T& v) { return this; }
        virtual void apply()
        {
            TImpl impl;
            impl.apply(Arg0<k,TImpl,T0>::arg0_,
                       Arg1<k,TImpl,T0,T1>::arg1_,
                       Arg2<k,TImpl,T0,T1,T2>::arg2_);
        }
    };

    //-------------------------------------------------------------------------

    // Attempt at using a single template for all arities
#if 0
    template <size_t k, typename TImpl,typename TMem, typename... Ts>
    struct Arg : public Arg<k-1,TImpl,Ts...>
    {
        TMem& arg_;
        Arg(Arg<k-1,TImpl,Ts...> prev, TMem& v) :
            Arg<k-1,TImpl,Ts...>(prev),
            arg_(v) {}
    };

    template <size_t k, typename TImpl, typename TMem, typename T, typename... Ts>
    struct Arg<k,TImpl,TMem, T, Ts...> : public Arg<k,TImpl,TMem,Ts...>
    {
        Arg<k,TImpl,TMem,T,Ts...>(Arg<k-1,TImpl,Ts...> prev, TMem& v) :
            Arg<k,TImpl,TMem,Ts...>(prev, v) {}
        using Arg<k,TImpl,TMem,Ts...>::addArg;
        virtual ArgList<TArgs...>* addArg(T& v);
    };

    template <typename TImpl>
    struct Arg<0,TImpl,> : public ArgList<TArgs...>,  public TImpl
    {
    };

    template <typename TImpl, typename T, typename... Ts>
    struct Arg<0, TImpl, T, Ts...> : public Arg<0,TImpl,Ts...>
    {
        using Arg<0,TImpl,Ts...>::addArg;
        virtual ArgList<TArgs...>* addArg(T& v);
    };
#endif
};


//----- Template method implementations ----------------------------------------

template <typename... TArgs>
template <size_t k, typename TImpl, typename T, typename... Ts>
Builder<TArgs...>::ArgList<TArgs...>*
Builder<TArgs...>::ArgStart<k,TImpl,T,Ts...>::addArg(T& v)
{
    return new Arg0<k,TImpl,T,TArgs...>(v);
}

template <typename... TArgs>
template <size_t k, typename TImpl, typename T0, typename T, typename... Ts>
Builder<TArgs...>::ArgList<TArgs...>*
Builder<TArgs...>::Arg0<k,TImpl,T0,T,Ts...>::addArg(T& v)
{
    return new Arg1<k,TImpl,T0,T,TArgs...>(Arg0<k,TImpl,T0>::arg0_,v);
}

template <typename... TArgs>
template <size_t k, typename TImpl, typename T0, typename T1, typename T, typename... Ts>
Builder<TArgs...>::ArgList<TArgs...>*
Builder<TArgs...>::Arg1<k,TImpl,T0,T1,T,Ts...>::addArg(T& v)
{
    return new Arg2<k,TImpl,T0,T1,T,TArgs...>(Arg0<k,TImpl,T0>::arg0_,
                                              Arg1<k,TImpl,T0,T1>::arg1_,v);
}

//----- Create a multimethod ---------------------------------------------------

struct Base
{
    template <typename T0>
    void apply(T0& arg0) {}

    template <typename T0, typename T1>
    void apply(T0& arg0, T1& arg1) {}

    template <typename T0, typename T1, typename T2>
    void apply(T0& arg0, T1& arg1, T2& arg2) {}

};

template <typename TImpl>
struct Mixin : public TImpl, public Base
{
    using TImpl::apply; // Highest precedence
    using Base::apply;
};

template <size_t k, typename... TArgs, typename TImpl>
typename Builder<TArgs...>::template ArgList<TArgs...>* method(TImpl& impl)
{
    return new typename Builder<TArgs...>::template ArgStart<k,Mixin<TImpl>,TArgs...>();
}

} // namespace multi
#endif // MULITMETHODS2_H

