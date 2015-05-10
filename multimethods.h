#ifndef MULTIMETHODS_H
#define MULTIMETHODS_H

namespace multi {

// TODO: make this generic
class ArgList {
public:
    virtual ~ArgList() {}
    virtual ArgList* addArg(int& v) = 0;
    virtual ArgList* addArg(double& v) = 0;
    virtual void apply() = 0;
};

template <class TImpl>
class Builder : public ArgList, public TImpl
{
public:
    virtual ArgList* addArg(int& v)    { return createArg0(v); }
    virtual ArgList* addArg(double& v) { return createArg0(v); }
    virtual void apply() {}
private:
    template <typename T0> ArgList* createArg0(T0& v);
};

template <typename TImpl, typename T0>
class Arg0 : public Builder<TImpl> {
protected:
    T0& arg0_;
public:
    Arg0(T0& v) : arg0_(v) {}
    virtual ArgList* addArg(int& v)    { return createArg1(v); }
    virtual ArgList* addArg(double& v) { return createArg1(v); }
    virtual void apply() {
        std::cout << arg0_ << std::endl;
    }
private:
    template <typename T1> ArgList* createArg1(T1& v);
};

template <typename TImpl, typename T0, typename T1>
class Arg1 : public Arg0<TImpl,T0> {
protected:
    T1& arg1_;
public:
    Arg1(Arg0<TImpl,T0>& arg0,T1& v) :
        Arg0<TImpl,T0>(arg0),
        arg1_(v) {}
    virtual ArgList* addArg(int& v)    { return createArg2(v); }
    virtual ArgList* addArg(double& v) { return createArg2(v); }
    virtual void apply() {
        std::cout << arg1_ << std::endl;
    }
private:
    template <typename T2> ArgList* createArg2(T2& v);
};

template <typename TImpl, typename T0, typename T1, typename T2>
class Arg2 : public Arg1<TImpl, T0, T1> {
protected:
  T2& arg2_;
public:
    Arg2(Arg1<TImpl,T0,T1>& arg1,T2& v) :
        Arg1<TImpl,T0,T1>(arg1),
        arg2_(v)
    {}

    virtual ArgList* addArg(int& v)    { return this; }
    virtual ArgList* addArg(double& v) { return this; }

    virtual void apply()
    {
        // Call Implementation
        TImpl::apply(Arg0<TImpl,T0>::arg0_,
                     Arg1<TImpl,T0, T1>::arg1_,
                     arg2_);
    }
};

//---- Template method implementations -----------------------------------------

template <typename TImpl>
template <typename T0>
ArgList* Builder<TImpl>::createArg0(T0& v)
{
    return new Arg0<TImpl,T0>(v);
}

template <typename TImpl, typename T0>
template <typename T1>
ArgList* Arg0<TImpl,T0>::createArg1(T1& v)
{
    return new Arg1<TImpl, T0, T1>(*this, v);
}

template <typename TImpl, typename T0, typename T1>
template <typename T2>
ArgList* Arg1<TImpl, T0, T1>::createArg2(T2& v)
{
    return new Arg2<TImpl,T0, T1, T2>(*this, v);
}

} // namespace multi
#endif // MULTIMETHODS_H

