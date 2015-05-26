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
        virtual void operator()() = 0;
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
    };

    //-------------------------------------------------------------------------

    template <size_t k, typename TImpl,typename... Ts> struct ArgStart :
            public ArgList<TArgs...>,
            public TImpl
    {
        virtual typename std::enable_if<k==0>::type operator()()
        {
            TImpl();
        }
    };

    template <size_t k, typename TImpl, typename T, typename... Ts>
    struct ArgStart<k, TImpl, T, Ts...> : public ArgStart<k,TImpl,Ts...>
    {
        using ArgStart<k,TImpl,Ts...>::addArg;
        virtual ArgList<TArgs...>* addArg(T& v);
    };

    //-------------------------------------------------------------------------

    template <size_t k, typename TImpl, typename T0, typename... Ts>
    struct Arg0 : public ArgList<TArgs...>
    {
        T0& arg0_;
        Arg0(T0& v) : arg0_(v) {}
        virtual typename std::enable_if<k==1>::type operator()()
        {
            TImpl(arg0_);
        }
    };

    template <size_t k, typename TImpl, typename T0, typename T, typename... Ts>
    struct Arg0<k, TImpl, T0, T, Ts...> : public Arg0<k,TImpl,T0,Ts...>
    {
        Arg0<k,TImpl,T0,T,Ts...>(T0& v) : Arg0<k,TImpl,T0,Ts...>(v) {}
        using Arg0<k,TImpl,T0,Ts...>::addArg;
        virtual ArgList<TArgs...>* addArg(T& v);
    };

    //-------------------------------------------------------------------------

    template <size_t k, typename TImpl, typename T0, typename T1, typename... Ts>
    struct Arg1 : public Arg0<k,TImpl,T0,Ts...>
    {
        T1& arg1_;
        Arg1(T0& arg0, T1& v) : Arg0<k,TImpl,T0,Ts...>(arg0), arg1_(v) {}
        virtual typename std::enable_if<k==2>::type operator()()
        {
            TImpl(Arg0<k,TImpl,T0>::arg0_,arg1_);
        }
    };

    template <size_t k, typename TImpl, typename T0, typename T1, typename T, typename... Ts>
    struct Arg1<k,TImpl,T0, T1, T, Ts...> : public Arg1<k,TImpl,T0,T1,Ts...>
    {
        Arg1<k,TImpl,T0,T1,T,Ts...>(T0& arg0, T1& v) : Arg1<k,TImpl,T0,T1,Ts...>(arg0,v) {}
        using Arg1<k,TImpl,T0,T1,Ts...>::addArg;
        virtual ArgList<TArgs...>* addArg(T& v);
    };

    //-------------------------------------------------------------------------

    template <size_t k, typename TImpl,typename T0, typename T1, typename T2, typename... Ts>
    struct Arg2 : public Arg1<k,TImpl,T0,T1,Ts...>
    {
        T2& arg2_;
        Arg2(T0& arg0, T1& arg1, T2& v) :
            Arg1<k,TImpl,T0,T1,Ts...>(arg0, arg1),
            arg2_(v) {}
        virtual std::enable_if<k==3,void> operator()()
        {
            TImpl(Arg0<k,TImpl,T0>::arg0_,
                  Arg1<k,TImpl,T0,T1>::arg1_,
                  arg2_);
        }
    };

    template <size_t k, typename TImpl,typename T0, typename T1, typename T2, typename T, typename... Ts>
    struct Arg2<k,TImpl,T0, T1, T2, T, Ts...> : public Arg2<k,TImpl,T0,T1,T2,Ts...>
    {
        Arg2<k,TImpl,T0,T1,T2,T,Ts...>(T0& arg0, T1& arg1, T2& v) :
            Arg2<k,TImpl,T0,T1,T2,Ts...>(arg0,arg1,v) {}
        using Arg2<k,TImpl,T0,T1,T2,Ts...>::addArg;
        virtual ArgList<TArgs...>* addArg(T& v) { return this; }
    };

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

template <size_t k, typename... TArgs, typename TImpl>
typename Builder<TArgs...>::template ArgList<TArgs...>* method(TImpl& impl)
{
    return new typename Builder<TArgs...>::template ArgStart<k,TImpl,TArgs...>();
}

} // namespace multi
#endif // MULITMETHODS2_H

