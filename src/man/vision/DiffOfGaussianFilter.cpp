#include "DiffOfGaussianFilter.h"

#include <cmath>
#include <iostream>
#include <limits>

namespace man {
namespace vision {

// TODO throw exception if ksize is even
// TODO precompute Difference of Gaussian kernels?
DiffOfGaussianFilter::DiffOfGaussianFilter(int ksize, double narrowSigma, double wideSigma)
    : size(ksize)
{
    // TODO refactor, copy and paste
    narrow = new double[size];
    wide = new double[size];
    nweights = new double[size];
    wweights = new double[size];

    // TODO eliminate unused space
    nweights[size-1] = 0;
    wweights[size-1] = 0;

    for (int i = 0; i < size; i++) {
        int x = i - (ksize/2);
        narrow[i] = DiffOfGaussianFilter::gaussianAt(static_cast<double>(x), narrowSigma);
        wide[i] = DiffOfGaussianFilter::gaussianAt(static_cast<double>(x), wideSigma);
        nweights[size-1] += narrow[i];
        wweights[size-1] += wide[i];
    }

    for (int i = size-2; i >= 0; i--) {
        nweights[i] = nweights[i+1] - narrow[size-2-i];
        wweights[i] = wweights[i+1] - wide[size-2-i];
    }
}

DiffOfGaussianFilter::~DiffOfGaussianFilter()
{
    delete[] narrow;
    delete[] wide;
    delete[] nweights;
    delete[] wweights;
}

void DiffOfGaussianFilter::convolve(int inSize, double const* in, double* out)
{
    int halfsize = size / 2;
    for (int i = 0; i < inSize; i++, out++) {
        int offset = i - halfsize;

        double nweight = 0;
        double wweight = 0;

        if (i >= halfsize && i < inSize - halfsize) {
            nweight = nweights[size-1];
            wweight = wweights[size-1];
        } else if (i < halfsize) {
            nweight = nweights[(size-1) - (halfsize-i)];
            wweight = wweights[(size-1) - (halfsize-i)];
        } else {
            nweight = nweights[(size-1) - (i - inSize + 1 + halfsize)];
            wweight = wweights[(size-1) - (i - inSize + 1 + halfsize)];
        }

        for (int j = 0; j < size; j++) {
            int index = offset + j;
            if (index >= 0 && index < inSize) {
                double kernelAtJ = (narrow[j]/nweight) - (wide[j]/wweight);
                *out += in[index]*kernelAtJ;
            }
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
