#pragma once

#include <algorithm>

namespace man {
namespace vision {

// NOTE Daniel, this has not been compiled or tested! Just wrote it based on Bill's C# stuff!

class Fool {
public:
    Fool(double value_) : value(value_) {}
    double get() const { return value; }
    inline Fool operator&(const Fool& f1, const Fool& f2) const;
    inline Fool operator|(const Fool& f1, const Fool& f2) const;
    inline Fool operator!(const Fool& f) const;

private:
    double value;
};

inline Fool Fool::operator&(const Fool& f1, const Fool& f2) const
{
    return Fool(std::min(f1.get(), f2.get())); 
}

inline Fool Fool::operator|(const Fool& f1, const Fool& f2) const
{
    return Fool(std::max(f1.get(), f2.get())); 
}

inline Fool Fool::operator!(const Fool& f) const
{
    return Fool(1 - f.get());
}

class FuzzyThr {
public:
    FuzzyThr(double t0_, double t1_) : t0(t0_), t1(t1_) {}
    inline double weight(double x) const; 
    inline Fool operator>(double x, const FuzzyThr& thr) const { return Fool(weight(x)); }
    inline Fool operator<(double x, const FuzzyThr& thr) const { return !(x > thr); }

private:
    double t0;
    double t1;
};

// TODO could be made more efficient, in particular are std::max and std::min efficient?
inline double FuzzyThr::weight(double x) const
{
    if (t0 == t1)
        return (x >= t0 ? 1. : 0.);
    return std::max(std::min((x - t0) / (t1 - t0), 0.), 1.);
}

inline Fool operator>(double x, const FuzzyThr& thr) const 
{ 
    return Fool(weight(x)); 
}

inline Fool operator<(double x, const FuzzyThr& thr) const 
{ 
    return !(x > thr); 
}

}
}
