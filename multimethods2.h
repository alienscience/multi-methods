#ifndef MULITMETHODS2_H
#define MULITMETHODS2_H

#include <memory>

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
        virtual ~ArgList() {}
        virtual ArgList<TArgs...>* addArg(const T& v) = 0;
    };

    template <typename T, typename... Ts>
    struct ArgList<T, Ts...> : public ArgList<Ts...>
    {
        // Premote functions from the base classes
        using ArgList<Ts...>::addArg;
        virtual ArgList<TArgs...>* addArg(const T& v) = 0;
        virtual void apply() = 0;
    };

    //-------------------------------------------------------------------------

    template <typename TImpl,typename... Ts> struct ArgStart :
            public ArgList<TArgs...>,
            public TImpl
    {
    };

    template <typename TImpl, typename T, typename... Ts>
    struct ArgStart<TImpl, T, Ts...> : public ArgStart<TImpl,Ts...>
    {
        using ArgStart<TImpl,Ts...>::addArg;
        virtual ArgList<TArgs...>* addArg(const T& v);
        virtual void apply(){}
    };

    //-------------------------------------------------------------------------

    template <typename TImpl, typename T0, typename... Ts>
    struct Arg0 : public ArgList<TArgs...>
    {
        T0 arg0_;
        Arg0(const T0& v) : arg0_(v) {}
    };

    template <typename TImpl, typename T0, typename T, typename... Ts>
    struct Arg0<TImpl, T0, T, Ts...> : public Arg0<TImpl,T0,Ts...>
    {
        Arg0<TImpl,T0,T,Ts...>(const T0& v) : Arg0<TImpl,T0,Ts...>(v) {}
        using Arg0<TImpl,T0,Ts...>::addArg;
        virtual ArgList<TArgs...>* addArg(const T& v);
        virtual void apply(){}
    };

    //-------------------------------------------------------------------------

    template <typename TImpl, typename T0, typename T1, typename... Ts>
    struct Arg1 : public Arg0<TImpl,T0,Ts...>
    {
        T1 arg1_;
        Arg1(const T0& arg0, const T1& v) :
            Arg0<TImpl,T0,Ts...>(arg0),
            arg1_(v) {}
    };

    template <typename TImpl, typename T0, typename T1, typename T, typename... Ts>
    struct Arg1<TImpl,T0, T1, T, Ts...> : public Arg1<TImpl,T0,T1,Ts...>
    {
        Arg1<TImpl,T0,T1,T,Ts...>(const T0& arg0, const T1& v) :
            Arg1<TImpl,T0,T1,Ts...>(arg0,v) {}

        using Arg1<TImpl,T0,T1,Ts...>::addArg;
        virtual ArgList<TArgs...>* addArg(const T& v);

        virtual void apply()
        {
            TImpl impl;
            impl.apply(Arg0<TImpl,T0>::arg0_, Arg1<TImpl,T0,T1>::arg1_);
        }
    };

    //-------------------------------------------------------------------------

    template <typename TImpl,typename T0, typename T1, typename T2, typename... Ts>
    struct Arg2 : public Arg1<TImpl,T0,T1,Ts...>
    {
        T2 arg2_;
        Arg2(const T0& arg0, const T1& arg1, const T2& v) :
            Arg1<TImpl,T0,T1,Ts...>(arg0, arg1),
            arg2_(v) {}
    };

    template <typename TImpl,typename T0, typename T1, typename T2, typename T, typename... Ts>
    struct Arg2<TImpl,T0, T1, T2, T, Ts...> : public Arg2<TImpl,T0,T1,T2,Ts...>
    {
        Arg2<TImpl,T0,T1,T2,T,Ts...>(const T0& arg0, const T1& arg1, const T2& v) :
            Arg2<TImpl,T0,T1,T2,Ts...>(arg0,arg1,v) {}
        using Arg2<TImpl,T0,T1,T2,Ts...>::addArg;
        virtual ArgList<TArgs...>* addArg(const T& v) { return this; }
        virtual void apply()
        {
            TImpl impl;
            impl.apply(Arg0<TImpl,T0>::arg0_,
                       Arg1<TImpl,T0,T1>::arg1_,
                       Arg2<TImpl,T0,T1,T2>::arg2_);
        }
    };

    //------------------------------------------------------------------------------

    struct Args
    {
        Args(ArgList<TArgs...>* args) : args_(args) {}

        template <typename T>
        void addArg(const T& v)
        {
            // The real type of args_ changes each time an argument is added
            args_ =  std::unique_ptr<ArgList<TArgs...>>(args_->addArg(v));
        }

        void apply()
        {
            args_->apply();
        }
    private:
        std::unique_ptr<ArgList<TArgs...>> args_;
    };

}; // Builder

//----- Template method implementations ----------------------------------------

template <typename... TArgs>
template <typename TImpl, typename T, typename... Ts>
Builder<TArgs...>::ArgList<TArgs...>*
Builder<TArgs...>::ArgStart<TImpl,T,Ts...>::addArg(const T& v)
{
    return new Arg0<TImpl,T,TArgs...>(v);
}

template <typename... TArgs>
template <typename TImpl, typename T0, typename T, typename... Ts>
Builder<TArgs...>::ArgList<TArgs...>*
Builder<TArgs...>::Arg0<TImpl,T0,T,Ts...>::addArg(const T& v)
{
    return new Arg1<TImpl,T0,T,TArgs...>(Arg0<TImpl,T0>::arg0_,v);
}

template <typename... TArgs>
template <typename TImpl, typename T0, typename T1, typename T, typename... Ts>
Builder<TArgs...>::ArgList<TArgs...>*
Builder<TArgs...>::Arg1<TImpl,T0,T1,T,Ts...>::addArg(const T& v)
{
    return new Arg2<TImpl,T0,T1,T,TArgs...>(Arg0<TImpl,T0>::arg0_,
                                              Arg1<TImpl,T0,T1>::arg1_,v);
}

//----- Decorate the implementation with unimplemented arities -----------------

struct Base
{
    virtual ~Base() {}

    template <typename T0>
    void apply(const T0& arg0) {}

    template <typename T0, typename T1>
    void apply(const T0& arg0, const T1& arg1) {}

    template <typename T0, typename T1, typename T2>
    void apply(const T0& arg0, const T1& arg1, const T2& arg2) {}

};

template <typename TImpl>
struct Mixin : public TImpl, public Base
{
    virtual ~Mixin() {}
    using TImpl::apply; // Highest precedence
    using Base::apply;
};

//----- Create a multimethod ---------------------------------------------------

template <typename TImpl, typename... TArgs>
typename Builder<TArgs...>::Args method()
{
    auto emptyArgs = new typename Builder<TArgs...>::template ArgStart<Mixin<TImpl>,TArgs...>();
    return typename Builder<TArgs...>::Args(emptyArgs);
}

} // namespace multi
#endif // MULITMETHODS2_H

