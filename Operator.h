
#ifndef Operator_h
#define Operator_h

#ifdef EXPRESSIONS

class Delta {
public:
    Delta()
    : first(true){}
        
    double operator()( double v2) throw(Expr::UnfindedVar)
    {
        if ( first) {
            first = false;
            v1 = v2;
            throw Expr::UnfindedVar();
        }
        double tmp = v2 - v1;
        v1 = v2;
        return tmp;
    }

private:
    double v1;
    bool first;
};

#endif //EXPRESSIONS

#endif
