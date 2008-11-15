/**
 * TODO: Put in some docs =)
 */

#ifndef _PreviewController_h_DEFINED
#define _PreviewController_h_DEFINED

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
using namespace boost::numeric;

#include "WalkController.h"

class PreviewController : public WalkController {
public:
    PreviewController();
    virtual const float tick(const float reference);
    virtual const float getPosition() const { return position; }

private:
    float position;

private: // Constants
    static const int NUM_PREVIEW_FRAMES = 40;
    static const float weights[NUM_PREVIEW_FRAMES];
    static const float A_c_values[9];
    static const float b_values[3];
    static const float c_values[3];

    /*
     * This saves me some nasty typing...
     * uBLAS by default uses std::vector as its underlying storage data
     * structure. By overriding the matrix type to use ublas::bounded_array
     * we're forcing uBLAS to use arrays as the storage type which allows all
     * variables to stay off the heap and on the stack. This offers a dramatic
     * increase in performance with the slight inconvenience that the static
     * array needs to be big enough to fit a n by n matrix. The biggest one I
     * handle is 3x3, so a bounded array of size 9 is sufficient.
     */
    typedef ublas::matrix<float,
                          ublas::row_major,
                          ublas::bounded_array<float,9> > ufmatrix3;
    typedef ublas::vector<float, ublas::bounded_array<float,3> > ufvector3;
    typedef ublas::matrix<float,
                          ublas::row_major,
                          ublas::bounded_array<float,3> > ufrowVector3;

    ufmatrix3 A_c;
    ufvector3 b;
    ufrowVector3 c;

    /*
    static const float R = 1.E-10;
    static const float Qe = 1.0f;
    static const float T = 0.02;
    static const float g = 9.8; // do not worry that gravity is positive
    */
};

#endif
