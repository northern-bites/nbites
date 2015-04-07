namespace man {
namespace vision {

// TODO partial kernels for dealing with edges of signal
class DiffOfGaussianFilter {
public:
    // TODO add constructor without kernel size param
    DiffOfGaussianFilter(int ksize, double narrowSigma, double wideSigma);
    ~DiffOfGaussianFilter();
    void convolve(int inSize, double const* in, double* out);

private:
    static double gaussianAt(double x, double sigma);

    int size;
    double* kernel;
};

}
}
