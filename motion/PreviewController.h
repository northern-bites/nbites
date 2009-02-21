/**
 * This class implements the 1D controller described by Kajita and Czarnetzki
 * Each discrete time step, the tick method is called with the latest
 * previewable ZMP_REF positions.
 * Important: This controller models only one dimension at once, so you need
 * two instances one for the x and one for the y direction.
 * The weights and the time invariant system matrix A_c (see constructor, etc)
 * are pre-calculated in Scilab (see preview-control.sci). The theory
 * is described in Czarnetzki and Kajita and Katayama.
 *
 * @author George Slavov
 * @author Johannes Strom
 * @date November 2008
 */

#ifndef _PreviewController_h_DEFINED
#define _PreviewController_h_DEFINED

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
using namespace boost::numeric;

#include <list>

#include "WalkController.h"
#include "motionconfig.h"

#ifdef DEBUG_MOTION
#  define DEBUG_CONTROLLER_GAINS
#endif

class PreviewController : public WalkController {
public:
    PreviewController();
    virtual ~PreviewController(){};
    virtual const float tick(const std::list<float> *zmp_ref);
    virtual const float getPosition() const { return stateVector(0); }
    virtual const float getZMP() const {return stateVector(2);}

    virtual void initState(float x, float v, float p);
private:
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

    ufvector3 stateVector;

public: //Constants
    static const unsigned int NUM_PREVIEW_FRAMES = 60;
private:
    static const float weights[NUM_PREVIEW_FRAMES];
    static const float A_c_values[9];
    static const float b_values[3];
    static const float c_values[3];

    ufmatrix3 A_c;
    ufvector3 b;
    ufrowVector3 c;

};

#endif
