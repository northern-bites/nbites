#include "DiffOfGaussianFilter.h"

#include <cmath>
#include <iostream>

namespace man {
namespace vision {

// TODO throw exception if ksize is even
DiffOfGaussianFilter::DiffOfGaussianFilter(int ksize, int narrowSigma, int wideSigma)
    : size(ksize)
{
    kernel = new double[size];

    double sum = 0;
    for (int i = 0; i < size; i++) {
        int x = i - (ksize/2);
        kernel[i] = DiffOfGaussianFilter::gaussianAt(x, narrowSigma) - 
                    DiffOfGaussianFilter::gaussianAt(x, wideSigma);
        sum += kernel[i];
    }

    for (int i = 0; i < size; i++) {
        kernel[i] = kernel[i] / sum;
    }
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

double DiffOfGaussianFilter::gaussianAt(int x, int sigma)
{	
	double denom = 1.0 / (sqrt(2.0*M_PI)*sigma);
	return denom * exp(-(x*x) / (2.0*sigma*sigma));
}

}
}
