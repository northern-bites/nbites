#include "BallDetector.h"


namespace man {
namespace vision {

BallDetector::BallDetector(messages::PackedImage8* orangeImage_):
orangeImage(orangeImage_)
{

}

BallDetector::~BallDetector() { }

void BallDetector::findBalls() {
    Blobber<uint8_t> b(orangeImage->pixelAddress(0, 0), orangeImage->width(),
                       orangeImage->height(), 1, orangeImage->width());

    b.run(NeighborRule::eight, 90, 100, 100, 100);

    blobs = b.getResult();
}

void BallDetector::rateBlob(Blob b) {
    double aspectRatio =  b.principalLength2() / b.principalLength1();

    b.setRating(aspectRatio * b.density());
}

}
}
