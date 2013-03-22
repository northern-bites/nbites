#ifndef HoughSpace_h_DEFINED
#define HoughSpace_h_DEFINED

#include <list>
#include <boost/shared_ptr.hpp>

#include "Gradient.h"
#include "geom/HoughLine.h"
#include "ActiveArray.h"
#include "HoughConstants.h"

/**
 * The accumulator space and associated functions for performing
 * the Hough Transform on a given set of gradient edge points.
 *
 * Returns the lines found in the image.
 */
class HoughSpace
{
public:
    static boost::shared_ptr<HoughSpace> create();

    /**
     * The main public interface for the HoughSpace class.
     * Finds all the lines in the image using the Hough Transform.
     */
    virtual std::list<std::pair<HoughLine, HoughLine> >
    findLines(Gradient& g) = 0;
    virtual uint16_t getHoughBin(int r, int t) = 0;

    void setAcceptThreshold(int t) { acceptThreshold = t;    }
    void setAngleSpread(int t)     { angleSpread     = t;    }

    int  getAngleSpread()          { return angleSpread;     }
    int  getAcceptThreshold()      { return acceptThreshold; }

protected:
    HoughSpace();
    virtual ~HoughSpace() { };

private:
    int acceptThreshold, angleSpread;
};

#endif /* HoughSpace_h_DEFINED */
