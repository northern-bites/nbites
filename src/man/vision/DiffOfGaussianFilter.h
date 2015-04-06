namespace man {
namespace vision {

// TODO partial kernels for dealing with edges of signal
class DiffOfGaussianFilter {
public:
    DiffOfGaussianFilter(int ksize, int narrowSigma, int wideSigma);
    ~DiffOfGaussianFilter();
    void convolve(int inSize, double const* in, double* out);

private:
    static double gaussianAt(int x, int sigma);

    int size;
    double* kernel;
};

}
}
