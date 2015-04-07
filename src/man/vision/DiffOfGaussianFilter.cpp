#include "DiffOfGaussianFilter.h"

#include <cmath>
#include <iostream>

namespace man {
namespace vision {

// TODO throw exception if ksize is even
DiffOfGaussianFilter::DiffOfGaussianFilter(int ksize, double narrowSigma, double wideSigma)
    : size(ksize)
{
    kernel = new double[size];

    double sumNarrow = 0;
    double sumWide = 0;

    double* narrow = new double[size];
    double* wide = new double[size];

    for (int i = 0; i < size; i++) {
        int x = i - (ksize/2);
        narrow[i] = DiffOfGaussianFilter::gaussianAt(static_cast<double>(x), narrowSigma);
        wide[i] = DiffOfGaussianFilter::gaussianAt(static_cast<double>(x), wideSigma);
        sumNarrow += narrow[i];
        sumWide += wide[i];
    }

    for (int i = 0; i < size; i++)
        kernel[i] = (narrow[i] / sumNarrow) - (wide[i] / sumWide);

    delete[] narrow;
    delete[] wide;
}

DiffOfGaussianFilter::~DiffOfGaussianFilter()
{
    delete[] kernel;
}

void DiffOfGaussianFilter::convolve(int inSize, double const* in, double* out)
{
    int halfsize = size / 2;
    out += halfsize;
    for (int i = halfsize; i < inSize - halfsize; i++, out++) {
        int offset = i - halfsize;
        for (int j = 0; j < size; j++) {
            *out += in[offset+j]*kernel[j];
        }
    }
}

double DiffOfGaussianFilter::gaussianAt(double x, double sigma)
{	
	double denom = 1.0 / (sqrt(2.0*M_PI)*sigma);
	return denom * exp(-(x*x) / (2.0*sigma*sigma));
}

}
}
