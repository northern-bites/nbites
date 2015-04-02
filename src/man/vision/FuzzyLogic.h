#pragma once

#include <algorithm>

namespace man {
namespace vision {

class Fool {
public:
    Fool(double value_) : value(value_) {}
    double get() const { return value; }
    inline Fool operator&(const Fool& f) const;
    inline Fool operator|(const Fool& f) const;
    inline Fool operator!() const;

private:
    double value;
};

inline Fool Fool::operator&(const Fool& f) const
{
    return Fool(std::min(f.get(), get()));
}

inline Fool Fool::operator|(const Fool& f) const
{
    return Fool(std::max(f.get(), get()));
}

inline Fool Fool::operator!() const
{
    return Fool(1 - get());
}

class FuzzyThreshold {
public:
    FuzzyThreshold(double t0_, double t1_) : t0(t0_), t1(t1_) {}
    inline double weight(double x) const;

private:
    double t0;
    double t1;
};

// TODO could be made more efficient, in particular are std::max and std::min efficient?
inline double FuzzyThreshold::weight(double x) const
{
    if (t0 == t1)
        return (x >= t0 ? 1. : 0.);
    return std::min(std::max((x - t0) / (t1 - t0), 0.), 1.);
}

inline Fool operator>(double x, const FuzzyThreshold& thr)
{
    return Fool(thr.weight(x));
}

inline Fool operator<(double x, const FuzzyThreshold& thr)
{
    return !(x > thr);
}

}
}
