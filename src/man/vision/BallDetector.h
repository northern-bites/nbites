#pragma once

#include "Images.h"
#include "Blobber.h"
#include "Blob.h"

#include <vector>

namespace man {
namespace vision {

class BallDetector {
public:
    BallDetector(messages::PackedImage8* orangeImage_);
    ~BallDetector();

    void findBalls();
private:
    void rateBlob(Blob b);

    messages::PackedImage8* orangeImage;
    std::vector<Blob> blobs;

};

}
}
