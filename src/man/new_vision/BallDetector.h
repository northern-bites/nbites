#pragma once

#include "Images.h"
#include "Blobber.h"
#include "Blob.h"

#include <vector>

namespace man {
namespace newVision {
class BallDetector {
public:
    Ball(PackedImage8* orangeImage);
    ~Ball() {};

private:
    void findBalls();

    PackedImage8* orangeImage;
    std::vector<Blob>* blobs;

};

}
}
