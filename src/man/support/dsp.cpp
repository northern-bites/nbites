#include <stdio.h>

#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "dsp.h"

// ********************************
// *                              *
// *  Filter Abstract Base Class  *
// *                              *
// ********************************

Filter::Filter()
{
    transientCount = 0;
    Reset();
}

// Destructor is not inline
Filter::~Filter()
{
}

void Filter::Reset()
{
    y = 0;
    sampleCount = 0;
}


// *******************
// *                 *
// *  Boxcar Filter  *
// *                 *
// *******************

// The ratio of cutoff wavelength to filter width is nearly constant
// over a wide range of wavelengths. This value was determined
// emperically in Excel.
const double Boxcar::cutRatio = 0.443;

Boxcar::Boxcar(int width)
{
    fifo.NullValue(0);
    Control(width);
    Reset();
}

Boxcar::Boxcar(double cutoff)
{
    fifo.NullValue(0);
    Cutoff(cutoff);
    Reset();
}

void Boxcar::Control(int w)
{
    fifo.MaxCount(w);
    TransientCount(w);
    Reset();
}

void Boxcar::Reset()
{
    Filter::Reset();
    fifo.Clear();
    sum    = 0;
    weight = 0;
}

double Boxcar::X(double x)
{
    if (fifo.Full())
        sum += x - fifo.Read();
    else
    {
        sum += x;
        weight = 1.0 / (fifo.Count() + 1);
    }
    fifo.Write(x);
    return Y(sum * weight);
}

double Boxcar::Mid() const
{
    // Client thinks Mid is const, but we need non-const access for peek, which
    // can change Underflows.
    FifoBuffer<double>& f = (FifoBuffer<double>&)fifo;
    int i = fifo.Count() >> 1;
    if ((fifo.MaxCount() & 1) == 0 && fifo.Count() > 1)
        return 0.5 * (f.Peek(i - 1) + f.Peek(i));
    else
        return f.Peek(i);
}

// ******************************
// *                            *
// *  One-Pole Low Pass Filter  *
// *                            *
// ******************************

// The formula for filter constant k as a function of the cutoff
// wavelength wsa determined from the Z transform.
void OnePoleLowPass::Control(double cutoff)
{
    wl = cutoff;
    if (wl > 0)
    {
        double b = 1.0 - cos(2 * M_PI / wl);
        k = sqrt(b * (b + 2.0)) - b;
    }
    else
        k = 1.0;

    TransientCount((int)wl);
}

double OnePoleLowPass::X(double x)
{
    double y = Filter::Y();
    return Y(y + k * (x - y));
}

// *****************************************************
// *                                                   *
// *  Two-Pole Butterworth (Critically Damped) Filter  *
// *                                                   *
// *****************************************************

Butterworth::Butterworth(double cutoff)
{
    Control(cutoff);
    v = 0;
}

// The formula for filter constant k as a function of the cutoff
// wavelength wsa determined from the Z transform.
void Butterworth::Control(double cutoff)
{
    wl = cutoff;
    if (wl > 0)
    {
        double b = 1.0 - cos(2 * M_PI / wl);
        k = (sqrt(b * (b + 2.0 * (M_SQRT2 - 1.0))) - b) / (M_SQRT2 - 1.0);
    }
    else
        k = 1.0;

    TransientCount((int)wl);
}

void Butterworth::Reset()
{
    Filter::Reset();
    v = 0;
}

double Butterworth::X(double x)
{
    double y = Filter::Y();
    v += k * (x - v);
    return Y(y + k * (v - y));
}

// This is called a template specialization. It allows us to define a
// specific implementation of a template member to be used when the
// specific class HighPass<Boxcar> is being instantiated. This is
// similar in purpose to overriding a base class member, but being a
// template mechanism it is done at compile time, not run time, and so
// it can be more efficient. For a Boxcar filter, we make a high-pass
// version not by subtracting the output from the current input value
// but from the mid-point input value.
template <>
double HighPass<Boxcar>::X(double x)
{
    Boxcar::X(x);
    return y = Mid() - Boxcar::Y();
}

// ********************
// *                  *
// *  Signal Monitor  *
// *                  *
// ********************

SignalMonitor::SignalMonitor(int numBins, double low, double high, bool log)
{
    bins = new int[numBins];
    SignalMonitor::numBins = numBins;

    if (log && low > 0 && high > 0) {
        // log histogram
        SignalMonitor::low  = ::log(low);
        SignalMonitor::high = ::log(high);
        SignalMonitor::log  = true;
    } else {
        // linear histogram
        SignalMonitor::low  = low;
        SignalMonitor::high = high;
        SignalMonitor::log  = false;
    }

    k = numBins / (SignalMonitor::high - SignalMonitor::low);
    Reset();
}

SignalMonitor::~SignalMonitor()
{
    delete [] bins;
}

void SignalMonitor::Reset()
{
    for (int i = 0; i < numBins; ++i)
        bins[i] = 0;
}

void SignalMonitor::X(double x)
{
    if (log) {
        if (x <= 0)
            x = low;
        else
            x = ::log(x);
    }

    int index = (int)((x - low) * k);

    if (index < 0)
        index = 0;
    else if (index >= numBins)
        index = numBins - 1;

    ++bins[index];
}

void SignalMonitor::Print()
{
	std::cout << toString();
}

/// @return a two-column string with bin midpoints (low to high) and counts
std::string SignalMonitor::toString()
{
	using namespace std;
	stringstream data;
	const int width = 8;

    for (int i = 0; i < numBins; ++i) {
		data << setw(width) << binMidPoint(i)  << binCount(i) << endl;
	}
	return data.str();
}

/// @return the number of samples that have been in this bin
const int SignalMonitor::binCount(int index) const {
	if (index < 0 || index > numBins)
		return 0;

	if (bins[index] > 0)
		return bins[index];
	else
		return 0;
}

/// @return the mid point of the bin at index
const double SignalMonitor::binMidPoint(int index) const {
	double x = (index + 0.5) / k + low;
	if (log)
		x = exp(x);
	return x;
}
