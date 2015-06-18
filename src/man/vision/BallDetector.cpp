#include "BallDetector.h"

#include <algorithm>
#include <iostream>


using std::to_string;

namespace man {
namespace vision {

BallDetector::BallDetector(FieldHomography* homography_, bool topCamera_):
    blobber(),
    homography(homography_),
    topCamera(topCamera_)
{
}

BallDetector::~BallDetector() { }

bool BallDetector::findBall(ImageLiteU8 orange)
{
#ifdef OFFLINE
    candidates.clear();
#endif

    blobber.run(orange.pixelAddr(), orange.width(), orange.height(), orange.pitch());

    // TODO: Sort blobber list by size
    for (auto i=blobber.blobs.begin(); i!=blobber.blobs.end(); i++) {
        double x_rel, y_rel;

        double bIX = ((*i).centerX() - orange.width()/2);
        double bIY = (orange.height() / 2 - (*i).centerY()) - (*i).firstPrincipalLength();

        bool belowHoriz = homography->fieldCoords(bIX, bIY, x_rel, y_rel);

        // This blob is above the horizon. Can't be a ball
        if (!belowHoriz) {
            //std::cout << "BLOB's above horizon:" << std::endl;
            continue;
        }

        Ball b((*i), x_rel, y_rel, orange.height(), orange.width());
        if (b.confidence() > .5) {
#ifdef OFFLINE
            candidates.push_back(b);
#endif
            if (b.confidence() > _best.confidence()) {
                _best = b;
            }
        }
        else {
            // TODO: ball debug flag
            //std::cout << "declined ball because:\n" << b.properties() << std::endl;
        }
    }
    if (best().confidence() > .5) {
        return true;
    }
    else {
        return false;
    }
}

Ball::Ball(Blob& b, double x_, double y_, int imgHeight_, int imgWidth_) :
    blob(b),
    radThresh(.25, .5),
    thresh(.5, .8),
    x_rel(x_),
    y_rel(y_),
    imgHeight(imgHeight_),
    imgWidth(imgWidth_),
    _confidence(0)
{
    std::cout << "Ball constructor" << std::endl;
    compute();
}

Ball::Ball() :
    blob(0),
    thresh(0, 0),
    radThresh(0, 0),
    _confidence(0)
{ }

void Ball::compute()
{
    std::cout << "BALL::compute()" << std::endl;

    dist = hypot(x_rel, y_rel);
    double density = blob.area() / blob.count();
    double aspectRatio = (blob.secondPrincipalLength() /
                          blob.firstPrincipalLength());

    expectedDiam = pixDiameterFromDist(dist);

    diameterRatio;
    if (expectedDiam > 2 * blob.firstPrincipalLength())
        diameterRatio = 2*blob.firstPrincipalLength() / expectedDiam;
    else
        diameterRatio = expectedDiam / (2 * blob.firstPrincipalLength());

    _confidence = (density > thresh).f() * (aspectRatio > thresh).f() * (diameterRatio > radThresh).f();
}

std::string Ball::properties()
{
    std::string d("====Ball properties:====\n");
    d += "\tRelativePosition: " + to_string(x_rel) + " "+ to_string( y_rel) + "\n";
    d += "\tHomographyDistance is: " + to_string(dist) + "\n";
    d += "\tdensity is: " + to_string(blob.area() / blob.count()) + "\n";
    d += "\tcount is: " + to_string(blob.count()) + "\n";
    d += "\tlocated: (" + to_string(blob.centerX()) +  ", " +
        to_string(blob.centerY()) + ")\n";
    d += "\taspectR is: " + to_string(blob.secondPrincipalLength() /
                                      blob.firstPrincipalLength()) + "\n";
    d += "\texpect ball to be this many pix: " + to_string(expectedDiam) + "\n";
    d += "\tdiamRatio: " + to_string(diameterRatio) + "\n";
    d += "\n\tconfidence is: " + to_string(_confidence) + "\n====================\n";
    return d;
}

// The expected diameter of ball in image at distance d in CM
double Ball::pixDiameterFromDist(double d) const
{
    double trig = atan(BALL_RADIUS / d);
    return 2 * imgHeight * trig / VERT_FOV_RAD;
}

}
}
