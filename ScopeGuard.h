
#ifndef ScopeGuard_h
#define ScopeGuard_h

class ScopeGuardImplBase
{
public:
    void Dismiss() const throw()
    {    dismissed_ = true;    }

protected:
    ScopeGuardImplBase() : dismissed_(false)
    {}
    ScopeGuardImplBase(const ScopeGuardImplBase& other)
    : dismissed_(other.dismissed_)
    {    other.Dismiss();    }
    ~ScopeGuardImplBase() {} // nonvirtual (see below why)
    mutable bool dismissed_;

private:
    ScopeGuardImplBase& operator=( const ScopeGuardImplBase&);
};

template <typename Fun, typename Parm>
class ScopeGuardImpl1 : public ScopeGuardImplBase
{
public:
    ScopeGuardImpl1(const Fun& fun, const Parm& parm)
    : fun_(fun), parm_(parm) 
    {}
    ~ScopeGuardImpl1()
    {
        if (!dismissed_) fun_(parm_);
    }
private:
    Fun fun_;
    const Parm parm_;
};

template <typename Fun, typename Parm>
ScopeGuardImpl1<Fun, Parm> makeGuard(const Fun& fun, const Parm& parm)
{
    return ScopeGuardImpl1<Fun, Parm>(fun, parm);
}

typedef const ScopeGuardImplBase& ScopeGuard;

template <class Obj, typename MemFun>
class ObjScopeGuardImpl0 : public ScopeGuardImplBase
{
public:
    ObjScopeGuardImpl0(Obj& obj, MemFun memFun)
    : obj_(obj), memFun_(memFun) 
    {}
    ~ObjScopeGuardImpl0()
    {
        if (!dismissed_) (obj_.*memFun_)();
    }
private:
    Obj& obj_;
    MemFun memFun_;
};

template <class Obj, typename MemFun>
ObjScopeGuardImpl0<Obj, MemFun> makeObjGuard(Obj& obj, MemFun fun)
{
    return ObjScopeGuardImpl0<Obj, MemFun>(obj, fun);
}

#endif //ScopeGuard_h
