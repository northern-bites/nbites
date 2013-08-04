// *****************
// *               *
// *  DSP Classes  *
// *               *
// *****************

#pragma once
#ifndef _dsp_
#define _dsp_

#include <string>


// The purpose of this code is to provide a simple, reasonably
// efficient discrete signal processing library, as well as a variety
// of C++ coding examples covering templates and polymorphic classes.

// **************************
// *                        *
// *  FIFO Buffer Template  *
// *                        *
// **************************
//
// A first-in first-out buffer template that stores items of type T,
// up to a client-adjustable maximum number of items. Reading an empty
// FIFO returns a client-specified NullValue and increments an error
// count. Writing a full FIFO discards the data and increments an
// error count. Error counts are used so that errors can be detected
// and reported, while not giving clients the added burden of checking
// for errors after each call and avoiding a drastic action like a
// throw that might crash the entire program.
//
// A null FIFO (maximum of zero items) is legal. It can be constructed
// by the default constructor and created by MaxCount(0). All read,
// write, and peek attempts on a null FIFO are errors and increment
// error counts as usual.

template <class T>
class FifoBuffer
{
public:
    // effect   Construct FIFO with specified maximum number of items.
    FifoBuffer(int maxCount = 0);
    ~FifoBuffer();

    // we have to define our own copy and assignment to make sure the ring gets copied
    // effect Construct a FIFO that is identical to other
    FifoBuffer(const FifoBuffer& other);
    // effect make this FIFO a copy of other
    FifoBuffer& operator= (const FifoBuffer& other);

    // effect   Get/set maximum number of items. Setting clears the FIFO.
    // notes    Setting zero items makes this a null FIFO.
    int  MaxCount() const { return ringSize;}
    void MaxCount(int n);

    // effect   Get/set the value to be returned when reading an empty
    //           FIFO or when calling Peek with an index that does not
    //           exist.
    // note     The value is undefined until set by the client, because
    //           since we don't know type T we can't pick a value.
    T    NullValue()    const { return nullValue;}
    void NullValue(T x)       { nullValue = x;   }

    // effect   Remove all items from the FIFO
    void Clear();

    // returns  True if the FIFO is empty/full.
    bool Empty() const { return count == 0;       }
    bool Full () const { return count == ringSize;}

    // returns  Number of items in the FIFO
    int Count() const { return count;}

    // returns  If the index is valid (0 <= index < Count()), the item at
    //           the specified index, numbered starting with 0 for the
    //           oldest item and up to Count()-1 for the newest.  If the
    //           index is invalid, NullValue()
    // effect   If the index is invalid, increment Underflows. This is why
    //           Peek is non-const.
    T Peek(int index);

    // returns  If the FIFO is not empty, the oldest element, otherwise NullValue
    // effect   If the FIFO is not empty remove the oldest element,
    //           otherwise increment Underflows
    T Read();

    // effect   If the FIFO is not full add the specified element,
    //           otherwise increment Overflows
    void Write(T);

    // returns  This FIFO
    // effect   Read or write using alternate syntax
    FifoBuffer& operator<< (T  x) { Write(x)  ; return *this;}
    FifoBuffer& operator>> (T& x) { x = Read(); return *this;}

    // returns  True if there have been any errors since construction
    bool Error() const { return (overflows > 0) | (underflows > 0);}

    // returns  Number of underflows/overflows since construction
    int Overflows () const { return overflows; }
    int Underflows() const { return underflows;}

private:
    // FIFO is implemented as a ring buffer
    T*  ring;
    int ringSize;       // number of items allocated
    int readIndex;
    int writeIndex;
    int count;
    T   nullValue;

    int overflows;
    int underflows;
};

// FIFO template class implementation. Template class inplementations
// must be in the header file, or #included by the header file, so the
// compiler will know how to instantiate a specific template when
// used.

template <class T>
FifoBuffer<T>::FifoBuffer(int maxCount)
{
    ring = 0;
    overflows = underflows = 0;
    MaxCount(maxCount);
}

template <class T>
FifoBuffer<T>::~FifoBuffer()
{
    delete [] ring;
}

// NOTE: accessing private members is allowed in this case because the copy
//       constructor is a class member function (access is by class, not by obj)
template <class T>
FifoBuffer<T>::FifoBuffer(const FifoBuffer& other) :
    ringSize(other.ringSize),
    readIndex(other.readIndex),
    writeIndex(other.writeIndex),
    count(other.count),
    nullValue(other.nullValue),
    overflows(other.overflows),
    underflows(other.underflows)
{
    ring = new T[ringSize];
    for (int i = 0; i < ringSize; ++i)
	ring[i] = other.ring[i];
}

template <class T>
FifoBuffer<T>& FifoBuffer<T>::operator= (const FifoBuffer& other)
{
    ringSize = other.ringSize;
    readIndex = other.readIndex;
    writeIndex = other.writeIndex;
    count = other.count;
    nullValue = other.nullValue;
    overflows = other.overflows;
    underflows = other.underflows;

    delete [] ring;
    ring = new T[ringSize];
    for (int i = 0; i < ringSize; ++i)
	ring[i] = other.ring[i];

    return *this;
}

template <class T>
void FifoBuffer<T>::MaxCount(int n)
{
    delete [] ring;
    if (n > 0)
        ring = new T[n];
    else
        ring = 0;
    ringSize = n;
    Clear();
}

template <class T>
void FifoBuffer<T>::Clear()
{
    readIndex = writeIndex = 0;
    count = 0;
}

template <class T>
T FifoBuffer<T>::Peek(int index)
{
    // This is equivalent to index < 0 || index >= count, but
    // faster. Conditional branches are slow.
    if ((unsigned int)index >= (unsigned int)count)
    {
        ++underflows;
        return nullValue;
    }

    int i = readIndex + index;
    if (i >= ringSize)    // don't use % operator, divide is slow
        i -= ringSize;
    return  ring[i];
}

template <class T>
T FifoBuffer<T>::Read()
{
    if (Empty())
    {
        ++underflows;
        return nullValue;
    }

    T x = ring[readIndex++];
    if (readIndex == ringSize)
        readIndex = 0;
    --count;
    return x;
}

template <class T>
void FifoBuffer<T>::Write(T x)
{
    if (Full())
        ++overflows;
    else
    {
        ring[writeIndex++] = x;
        if (writeIndex == ringSize)
            writeIndex = 0;
        ++count;
    }
}

// ********************************
// *                              *
// *  Filter Abstract Base Class  *
// *                              *
// ********************************
//
// A Filter is a polymorphic object that operates on an input sequence
// X of doubles and produces an output sequence Y of doubles. The
// intent is to provide a base class for a variety of discrete
// filters. A Filter can be Reset to restore its state to the initial
// value (i.e. erase all history). All Filters are in the reset state
// when constructed.
//
// This class is abstract because it contains at least one pure
// virtual function (defined by "= 0"). No instance of an abstract
// class can be created. To make a concrete Filter, derive from this
// base class and implement the virtual members. See below for
// examples.
//
// Some filters are defined as a template <class F>, where F is some
// class derived from Filter. This is explained in some examples
// below. In order for a Filter to be used as a template argument in
// the filter library, it must have the following. Note that any of
// these can be provided directly by the class or inherited from a
// base class, except for the constructors which of course are not
// inherited.
//
//        * Filter as a direct or indirect base class
//        * A nested type (struct, class, union, or typedef) named
//          "ControlType" that defines a value that can be used to
//          control the filter
//        * A default constructor
//        * A constructor with one argument of type ControlType
//        * A member function "ControlType Control() const" that
//          returns the Filter's current control value
//        * A member function "void Control(ControlType)" that sets
//          the Filter's control value

class Filter
{
public:
    // effect   Construct in reset state
    Filter();

    // A polymorphic base class should have a virtual destructor.
    virtual ~Filter();

    // effect   Restore state to the initial value (i.e. erase all history).
    // note     Derived classes will often override this, but should call it
    //           in the overriding member
    virtual void Reset();

    // returns  The next output value
    // effect   Specify next input value, update filter state.
    // note     Derived classes must call the protected Y(double) member to
    //           generate the output, e.g. "return Y(someValue)"
    virtual double X(double) = 0;

    // returns  The current output value
    // note     This is the same value returned by the most recent call to
    //           X(). Typically derived classes don't need to override
    //           this, but see HighPass for an example of doing so.
    virtual double Y() const { return y;}

    // returns  Number of inputs after Reset for filter to be beyond
    //           transient startup phase.
    int TransientCount() const { return transientCount;}

    // returns  True if filter is in steady state, i.e. sufficient inputs
    //           have been received
    bool Steady() const { return sampleCount >= transientCount;}

    // returns  Number of inputs since Reset
    int SampleCount() const { return sampleCount;}

protected:
    // effect   Set transient count. Derived classes must call this in the
    //           constructor and when it changes
    void TransientCount(int tc) { transientCount = tc;}

    // effect   Set current output value. Derived classes call this in the X() member.
    double Y(double y) {  ++sampleCount; return Filter::y = y;}

private:
    double y;                 // current output
    int    transientCount;
    int    sampleCount;       // number of samples since Reset
};

// *******************
// *                 *
// *  Boxcar Filter  *
// *                 *
// *******************
//
// A Boxcar is a type of Filter that implements an FIR filter with
// uniform weights over a client-specified Width. This is a
// constant-time filter, i.e. the execution time is independent of
// Width.
//
// The output value is the mean of the last Width input values, or all
// of the input values if there have been fewer than Width values. The
// code is written so that no divides are done once Width values have
// been given, since divides are slower than multiplies.

class Boxcar : public Filter
{
public:
    // Filter width
    typedef int ControlType;

    // Default and ControlType constructor
    Boxcar(int width = 0);

    // effect   Construct with specified cutoff wavelength
    Boxcar(double cutoff);

    // effect   Get/set cutoff wavelength
    // note     Setting the value resets the filter
    double Cutoff(         ) const { return Control() / cutRatio;}
    void   Cutoff(double wl)       { Control((int)(wl * cutRatio + 0.5)); }

    // effect   Get/set control value (width of filter)
    // note     Setting the control value resets the filter
    int  Control(     ) const { return fifo.MaxCount(); }
    void Control(int w);

    virtual void Reset();

    virtual double X(double);

    // returns  The input value (x) at the mid-point of the most recent Width values.

    // note     If Width is odd, the returned value is the single value at
    //          the exact mid-point. If Width is even, the returned
    //          value is the average of the two values surrounding the
    //          exact mid-point, so the Mid value is somewhat
    //          smoothed. If Width is even but there have only been an
    //          odd number of x values, the returned value is still the
    //          average of two values, the exact mid-point and one
    //          neighbor, to keep the same smoothing, unless there has
    //          been only one x value so far, in which case it is
    //          returned.
    double Mid() const;

private:
    FifoBuffer<double> fifo;

    double sum;       // sum of values currently in the FIFO
    double weight;    // reciprocal of number of samples currently in the FIFO

    static const double cutRatio;
};

// ******************************
// *                            *
// *  One-Pole Low Pass Filter  *
// *                            *
// ******************************
//
// A one-pole low pass filter. The control value is the cutoff
// wavelength, i.e. the period of an input sine wave whose power
// (squared amplitude) will be reduced by a factor of 2. Wavelengths
// below this value are strongly attenuated, and those above are
// little attenuated. As a special case, if the control value is 0 the
// filter passes all input unchanged.
class OnePoleLowPass : public Filter
{
public:
    typedef double ControlType;

    // Default and ControlType constructor
    OnePoleLowPass(double cutoff = 0) { Control(cutoff); }

    // effect   Get/set cutoff wavelength
    double Control(      ) const { return wl;}
    void   Control(double);

    virtual double X(double x);

private:
    double wl;  // control value (cutoff wavelength in samples)
    double k;   // filter constant
};

// *****************************************************
// *                                                   *
// *  Two-Pole Butterworth (Critically Damped) Filter  *
// *                                                   *
// *****************************************************
//
// A two-pole low pass filter. This is equivalent to a composition of
// two identical one-pole filters, but with filter constant chosen to
// get the specified cutoff wavelength for the composition. Such a
// filter is an example of a Butterworth filter, and is critically
// damped.

class Butterworth : public Filter
{
public:
    typedef double ControlType;

    // Default and ControlType constructor
    Butterworth(double cutoff = 0);

    // effect   Get/set cutoff wavelength
    double Control(      ) const { return wl; }
    void   Control(double);

    virtual void Reset();

    virtual double X(double x);

private:
    double wl;      // control value (cutoff wavelength in samples)
    double k;       // filter constant
    double v;       // intermediate filter output
};

// **********************
// *                    *
// *  High-Pass Filter  *
// *                    *
// **********************
//
// A high-pass filter can be made from any low-pass filter by
// subtracting the low-pass filtered values from the input values. The
// most convenient way to make such a filter is to derive a class from
// an existing low-pass filter class, and then override the virtual
// member X. This is nice because the new class inherits various
// members from its base class that don't have to be reimplemented
// (e.g. Control). But this requires deriving a new class for every
// low-pass filter class from which we want to make a high-pass
// filter.
//
// A universal high-pass filter class can be made using the
// polymorphic properties of Filter. Such a class would have a Filter*
// as a member, which can point to any Filter. This class would also
// derive from Filter, and implement all of the virtual members to use
// the Filter* member. The Filter* could be specified by the client
// when the high-pass class is constructed. The disadvantages of this
// method is that every virtual member of Filter needs to be
// implemented, and also the run-time code is somewhat inefficient
// because the compiler doesn't know what kind of Filter will be
// pointed to by the Filter* member.
//
// The best solution is to make the high-pass filter a template class,
// with the specific low-pass filter on which it is to be based used
// as the template argument. This allows the convenience of deriving
// the high-pass filter from a low-pass base class, with the template
// mechanism automatically creating a high- pass filter from and
// low-pass type that is used. Since the compiler knows the specific
// low-pass type, more efficient code can be generated.

template <class F>
class HighPass : public F
{
public:
    // ControlType is inherited from F

    // Default and ControlType constructor
    HighPass();
    HighPass(typename F::ControlType);

    virtual void Reset();

    // Here is where the high-pass is done
    virtual double X(double x) { return y = x - F::X(x); }

    // Need to override Y() to return HighPass::y rather than base Filter::y
    virtual double Y() const { return y;}

private:
    double y;   // need my own output, hides base class y
};

// To construct me, just construct the base class and clear y.
template <class F>
HighPass<F>::HighPass()
{
    y = 0;
}

template <class F>
HighPass<F>::HighPass(typename F::ControlType c)
    : F(c)
{
    y = 0;
}

// My reset is just the base class reset, and clear y
template <class F>
void HighPass<F>::Reset()
{
    F::Reset();
    y = 0;
}

// *********************
// *                   *
// *  Variance Filter  *
// *                   *
// *********************
//
// Here is another example of a Filter that is a template class. A
// variance filter needs two identical low-pass filters, one for the
// signal and one for the signal squared. This could be done with two
// Filter* members created by the client, but there would be no way to
// insure that the Filters are identical.  By using a template, we can
// create and manage two identical Filters without burdening the
// client.

template <class F>
class VarianceFilter : public Filter
{
public:
    // ControlType is not inherited, so it has to be defined
    typedef typename F::ControlType ControlType;

    // Default and ControlType constructor
    VarianceFilter();
    VarianceFilter(ControlType);

    virtual void Reset();

    // effect   Get/set control value
    ControlType Control(           ) const { return sig.Control();}
    void        Control(ControlType);

    virtual double X(double);

    // returns  Standard deviation of signal
    double SD() const { return sqrt(Y());}

private:
    F sig;
    F sigSquared;
};

template <class F>
VarianceFilter<F>::VarianceFilter()
{
}

template <class F>
VarianceFilter<F>::VarianceFilter(ControlType c)
    : sig(c), sigSquared(c)
{
    TransientCount(sig.TransientCount());
}

template <class F>
void VarianceFilter<F>::Reset()
{
    Filter::   Reset();
    sig       .Reset();
    sigSquared.Reset();
}

template <class F>
void VarianceFilter<F>::Control(ControlType c)
{
    sig       .Control(c);
    sigSquared.Control(c);
    TransientCount(sig.TransientCount());
}

template <class F>
double VarianceFilter<F>::X(double x)
{
    double s = sig.X(x);
    return Y(sigSquared.X(x * x) - s * s);
}

// ************************
// *                      *
// *  Filter Composition  *
// *                      *
// ************************
//
// This template class allows two Filters to be composed, i.e. strung
// together to create one Filter by connecting the output of one to
// the input of the other. This could have been done using Filter*
// members, but using a template instead allows compile-time knowledge
// of the types, which is more efficient. It also permits type-safe,
// easy to implement, control of the composed filter. Also, there is
// no issue about who owns the Filters (i.e. the class or the client),
// which prevents all manner of programming errors (e.g. who is
// responsible for deleting Filters).
//
// One can use this template to compose any number of Filters, e.g.
//   ComposedFilter<ComposedFilter<F1, F2>, ComposedFilter<F3, F4>> fourFilters;

template <class F1, class F2>
class ComposedFilter: public Filter
{
public:
    // The ControlType is a struct with data members for the two control values
    struct ControlType
    {
        typename F1::ControlType C1;
        typename F2::ControlType C2;

        // A syntactically convenient way to specify a control value
        ControlType(typename F1::ControlType c1, typename F2::ControlType c2)
            { C1 = c1; C2 = c2; }
    };

    // Default and ControlType constructor
    ComposedFilter();
    ComposedFilter(ControlType);

    virtual void Reset();

    // effect   Get/set control value
    ControlType Control(           ) const { return ControlType(f1.Control(), f2.Control()); }
    void        Control(ControlType);

    virtual double X(double x) { return Y(f2.X(f1.X(x))); }

    // returns Const references to each component Filter, which can be
    // use to read their states.
    const F1& A() const { return f1;}
    const F2& B() const { return f2;}

private:
    F1 f1;
    F2 f2;
};

template <class F1, class F2>
ComposedFilter<F1, F2>::ComposedFilter()
{
}

template <class F1, class F2>
ComposedFilter<F1, F2>::ComposedFilter(ControlType c)
{
    Control(c);
}

template <class F1, class F2>
void ComposedFilter<F1, F2>::Control(ControlType c)
{
    f1.Control(c.C1);
    f2.Control(c.C2);
    TransientCount(f1.TransientCount() + f2.TransientCount());
}

template <class F1, class F2>
void ComposedFilter<F1, F2>::Reset()
{
    Filter::Reset();
    f1     .Reset();
    f2     .Reset();
}


// *****************
// *               *
// *  Noise Meter  *
// *               *
// *****************
//
// A NoiseMeter is a composition of a Boxcar high-pass filter and a
// VarianceFilter. Notice how simple this is to create. The
// constructor does all the work, which is hardly any.
template <class F>
class NoiseMeter : public ComposedFilter<HighPass<Boxcar>, VarianceFilter<F> >
{
public:
    // A shortcut name for the inherited ControlType
    typedef typename ComposedFilter<HighPass<Boxcar>, VarianceFilter<F> >::ControlType ControlType;

    // Default and ControlType constructor
    NoiseMeter();
    NoiseMeter(ControlType);
};

template <class F>
NoiseMeter<F>::NoiseMeter()
{
}

template <class F>
NoiseMeter<F>::NoiseMeter(ControlType c)
    : ComposedFilter<HighPass<Boxcar>, VarianceFilter<F> >(c)
{
}

// ********************
// *                  *
// *  Signal Monitor  *
// *                  *
// ********************
//
// Monitor any signal and keep a histogram of its values for later analysis.

class SignalMonitor
{
public:
    // effect   Construct as specified
    // args     numBins   Number of bins in the histogram
    //          low       The signal value corresponding to the low value of
    //                     the first histogram bin
    //          high      The signal value corresponding to the high value of
    //                     the last histogram bin
    //          log       The histogram is log (true) or linear (false)
    // note     Signal values than are below "low" are counted in the first
    //           bin; those above "high" are counted in the last bin

    SignalMonitor(int numBins, double low, double high, bool log);
    ~SignalMonitor();

    // effect   Increment the histogram bin corresponding to the specified signal value
    void X(double);

    // effect   Clear the histogram
    void Reset();

    const int NumberOfBins() const { return numBins; }
    const int binCount(int index) const;
    const double binMidPoint(int index) const;

    // effect   Print the histogram, using the specified format string for
    //           printing signal values
    void Print();
    std::string toString();

private:
    int*    bins;       // the histogram
    int     numBins;
    double  low, high;
    bool    log;
    double  k;          // Convert from signal value to bin index with just a multiply
};


#endif
