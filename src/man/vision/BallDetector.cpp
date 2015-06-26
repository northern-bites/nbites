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
    blobber.secondThreshold(115);
    blobber.minWeight(4);
}

BallDetector::~BallDetector() { }

bool BallDetector::findBall(ImageLiteU8 orange, double cameraHeight)
{
#ifdef OFFLINE
    candidates.clear();
#endif

    blobber.run(orange.pixelAddr(), orange.width(), orange.height(), orange.pitch());

    Ball reset;
    _best = reset;

    // TODO: Sort blobber list by size
    for (auto i=blobber.blobs.begin(); i!=blobber.blobs.end(); i++) {
        double x_rel, y_rel;

        double bIX = ((*i).centerX() - orange.width()/2);
        double bIY = (orange.height() / 2 - (*i).centerY()) - (*i).firstPrincipalLength();

        bool belowHoriz = homography->fieldCoords(bIX, bIY, x_rel, y_rel);

        // This blob is above the horizon. Can't be a ball
        if (!belowHoriz) {
#ifdef OFFLINE
            std::cout << "BLOB's above horizon:" << std::endl;
#endif
            continue;
        }

        Ball b((*i), x_rel, -1 * y_rel, cameraHeight, orange.height(), orange.width());
        if (b.confidence() > .5) {
#ifdef OFFLINE
            candidates.push_back(b);
            std::cout << "accepted ball because:\n" << b.properties() << std::endl;
#endif
            if (b.dist < _best.dist) {
                _best = b;
            }
        }
        else {
            // TODO: ball debug flag
#ifdef OFFLINE
            std::cout << "declined ball because:\n" << b.properties() << std::endl;
#endif
        }
    }
    if (_best.confidence() > .5) {
        return true;
    }
    else {
        return false;
    }
}

Ball::Ball(Blob& b, double x_, double y_, double cameraH_, int imgHeight_, int imgWidth_) :
    blob(b),
    radThresh(.3, .7),
    thresh(.5, .8),
    x_rel(x_),
    y_rel(y_),
    cameraH(cameraH_),
    imgHeight(imgHeight_),
    imgWidth(imgWidth_),
    _confidence(0)
{
    compute();
}

Ball::Ball() :
    blob(0),
    thresh(0, 0),
    radThresh(0, 0),
    _confidence(0),
    dist(200000)
{ }

void Ball::compute()
{
    dist = hypot(x_rel, y_rel);
    double density = blob.area() / blob.count();
    double aspectRatio = (blob.secondPrincipalLength() /
                          blob.firstPrincipalLength());

    double hypotDist = hypot(dist, cameraH);

    expectedDiam = pixDiameterFromDist(hypotDist);

    diameterRatio;
    if (expectedDiam > 2 * blob.firstPrincipalLength())
        diameterRatio = 2*blob.firstPrincipalLength() / expectedDiam;
    else
        diameterRatio = expectedDiam / (2 * blob.firstPrincipalLength());

    //_confidence = (density > thresh).f() * (aspectRatio > thresh).f() * (diameterRatio > radThresh).f();
    _confidence = ((density > thresh) & (aspectRatio > thresh) & (diameterRatio > radThresh)).f();
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
    d += "\tprinceLens: " + to_string(blob.firstPrincipalLength()) + " " +
        to_string(blob.secondPrincipalLength()) + "\n";
    d += "\taspectR is: " + to_string(blob.secondPrincipalLength() /
                                      blob.firstPrincipalLength()) + "\n";
    d += "\texpect ball to be this many pix: " + to_string(expectedDiam) + "\n";
    d += "\tdiamRatio: " + to_string(diameterRatio) + "\n";
    d += "\tdiam Confidence: " + to_string((diameterRatio> radThresh).f()) + "\n";
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
