#pragma once

namespace man {
namespace vision {

class DiffOfGaussianFilter {
public:
    // TODO add constructor without kernel size param?
    DiffOfGaussianFilter(int ksize, double narrowSigma, double wideSigma);
    ~DiffOfGaussianFilter();
    void convolve(int inSize, double const* in, double* out);

private:
    static double gaussianAt(double x, double sigma);

    int size;
    double* narrow;
    double* wide;
    double* nweights;
    double* wweights;
};

}
}
