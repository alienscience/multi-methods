#ifndef MULITMETHODS_H
#define MULITMETHODS_H

/**
    The MIT License (MIT)

    Copyright (c) 2015 Saul Hazledine

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
**/

#include <memory>

namespace multi {

// Builder is a container template that holds all the possible types
// of multimethod arguments
// TImpl - Class containing method implementation
// TArgs - Possible types
template <typename TImpl, typename... TArgs>
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

    template <typename... Ts>
    struct ArgStart :
            public ArgList<TArgs...>,
            public TImpl
    {
        ArgStart(const TImpl& impl) : TImpl(impl) {}
    };

    template <typename T, typename... Ts>
    struct ArgStart<T, Ts...> : public ArgStart<Ts...>
    {
        ArgStart(const TImpl& impl) : ArgStart<Ts...>(impl) {}

        using ArgStart<Ts...>::addArg;
        virtual ArgList<TArgs...>* addArg(const T& v);
        virtual void apply(){}
    };

    //-------------------------------------------------------------------------

    template <typename T0, typename... Ts>
    struct Arg0 : public ArgStart<>
    {
        T0 arg0_;
        Arg0(const ArgStart<>& argStart, const T0& v) :
            ArgStart<>(argStart),
            arg0_(v) {}
    };

    template <typename T0, typename T, typename... Ts>
    struct Arg0<T0, T, Ts...> : public Arg0<T0,Ts...>
    {
        Arg0<T0,T,Ts...>(const ArgStart<>& argStart, const T0& v) :
            Arg0<T0,Ts...>(argStart,v) {}

        using Arg0<T0,Ts...>::addArg;
        virtual ArgList<TArgs...>* addArg(const T& v);
        void apply()
        {
            TImpl::apply(Arg0<T0>::arg0_);
        }
    };

    //-------------------------------------------------------------------------

    template <typename T0, typename T1, typename... Ts>
    struct Arg1 : public Arg0<T0>
    {
        T1 arg1_;
        Arg1(const Arg0<T0>& arg0, const T1& v) :
            Arg0<T0>(arg0),
            arg1_(v) {}
    };

    template <typename T0, typename T1, typename T, typename... Ts>
    struct Arg1<T0, T1, T, Ts...> : public Arg1<T0,T1,Ts...>
    {
        Arg1<T0,T1,T,Ts...>(const Arg0<T0>& arg0, const T1& v) :
            Arg1<T0,T1,Ts...>(arg0,v) {}

        using Arg1<T0,T1,Ts...>::addArg;
        virtual ArgList<TArgs...>* addArg(const T& v);
        virtual void apply()
        {
            TImpl::apply(Arg0<T0>::arg0_, Arg1<T0,T1>::arg1_);
        }
    };

    //-------------------------------------------------------------------------

    template <typename T0, typename T1, typename T2, typename... Ts>
    struct Arg2 : public Arg1<T0,T1>
    {
        T2 arg2_;
        Arg2(const Arg1<T0,T1>& arg1, const T2& v) :
            Arg1<T0,T1>(arg1),
            arg2_(v) {}
    };

    template <typename T0, typename T1, typename T2, typename T, typename... Ts>
    struct Arg2<T0, T1, T2, T, Ts...> : public Arg2<T0,T1,T2,Ts...>
    {
        Arg2<T0,T1,T2,T,Ts...>(const Arg1<T0,T1>& arg1, const T2& v) :
            Arg2<T0,T1,T2,Ts...>(arg1,v) {}
        using Arg2<T0,T1,T2,Ts...>::addArg;
        virtual ArgList<TArgs...>* addArg(const T& v) { return this; }
        virtual void apply()
        {
            TImpl::apply(Arg0<T0>::arg0_,
                         Arg1<T0,T1>::arg1_,
                         Arg2<T0,T1,T2>::arg2_);
        }
    };

    //------------------------------------------------------------------------------

    template <typename TState>
    struct Args
    {
        Args(TState& state, ArgList<TArgs...>* args) :
            state_(state),
            args_(args) {}

        template <typename T>
        void addArg(const T& v)
        {
            // The real type of args_ changes each time an argument is added
            args_ =  std::unique_ptr<ArgList<TArgs...>>(args_->addArg(v));
        }

        void apply()
        {
            args_->apply();
            // TODO: clean this up - it should be a static_cast
            state_ = TState(dynamic_cast<TState&>(*args_));
        }
    private:
        TState& state_;
        std::unique_ptr<ArgList<TArgs...>> args_;
    };

}; // Builder

//----- Template method implementations ----------------------------------------

template <typename TImpl, typename... TArgs>
template <typename T, typename... Ts>
Builder<TImpl,TArgs...>::ArgList<TArgs...>*
Builder<TImpl,TArgs...>::ArgStart<T,Ts...>::addArg(const T& v)
{
    return new Arg0<T,TArgs...>(*this,v);
}

template <typename TImpl, typename... TArgs>
template <typename T0, typename T, typename... Ts>
Builder<TImpl,TArgs...>::ArgList<TArgs...>*
Builder<TImpl,TArgs...>::Arg0<T0,T,Ts...>::addArg(const T& v)
{
    return new Arg1<T0,T,TArgs...>(*this,v);
}

template <typename TImpl, typename... TArgs>
template <typename T0, typename T1, typename T, typename... Ts>
Builder<TImpl,TArgs...>::ArgList<TArgs...>*
Builder<TImpl,TArgs...>::Arg1<T0,T1,T,Ts...>::addArg(const T& v)
{
    return new Arg2<T0,T1,T,TArgs...>(*this,v);
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
    Mixin(const TImpl& impl) : TImpl(impl) {}
    virtual ~Mixin() {}
    TImpl impl()  { return static_cast<TImpl>(this); }

    using TImpl::apply; // Highest precedence
    using Base::apply;
};

//----- Create a multimethod ---------------------------------------------------

template <typename... TArgs, typename TImpl>
typename Builder<Mixin<TImpl>,TArgs...>::template Args<TImpl> method(TImpl& impl)
{
    Mixin<TImpl> wrappedImpl(impl);
    auto* emptyArgs = new typename Builder<Mixin<TImpl>,TArgs...>::template ArgStart<TArgs...>(wrappedImpl);
    return typename Builder<Mixin<TImpl>,TArgs...>::template Args<TImpl>(impl,emptyArgs);
}

} // namespace multi
#endif // MULITMETHODS_H

