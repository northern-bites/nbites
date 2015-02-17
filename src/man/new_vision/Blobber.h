#pragma once

#include <iostream>
#include <list>
#include <vector>

#include "Blob.h"

// TODO add histeresis thresholding
// TODO more serious testing
// TODO add documentation

namespace man {
namespace vision {

enum class NeighborRule { four, eight };

template <typename T>
class Blobber {
public:
    Blobber(T const *pixels_, int width_, int height_, 
            int pixelPitch_, int rowPitch_);
    ~Blobber();
    void run(NeighborRule rule, double lowThreshold, 
             double highThreshold, double minArea);
    std::vector<Blob>& getResult() { return results; }

private:
    void initializeMark();
    void initializeNeighborRule(NeighborRule rule);
    // TODO pass efficient threshold object, instead of just low and high doubles
    // NOTE Daniel, FuzzyLogic.h includes such an object, we are using this
    //      in hough stuff, so makes sense share code
    inline void explore(Blob &blob, T const *p, bool *m, 
                        double lowThreshold, double highThreshold);
    inline int calculateXIndex(T const *pixelPt);
    inline int calculateYIndex(T const *pixelPt);
    inline double weight(double value, double lowThreshold, double highThreshold);
    
    // TODO refactor into image struct?
    T const *pixels;
    int width;
    int height;
    int pixelPitch;
    int rowPitch;

    bool *mark;
    // TODO stop using stl::list
    // TODO stop using stl::vector
    std::list<T const *> list;
    std::vector<Blob> results;

    int neighbors;
    int table[8];
};

}
}

int main(int argc, char **argv)
{
    std::cout << "Testing Blobber.\n";

    char pixels[25] = {10, 10, 10, 90, 10, 
                       0, 50, 50, 0, 0, 
                       0, 90, 90, 90, 0, 
                       40, 0, 50, 0, 0, 
                       90, 90, 0, 0, 0};

    man::vision::Blobber<char> blobber(pixels, 5, 5, 1, 5);
    blobber.run(man::vision::NeighborRule::four, 30, 60, 2); 

    std::vector<man::vision::Blob> results = blobber.getResult();
    std::cout << "How many blobs? " << results.size() << std::endl;
    for (unsigned int i = 0; i < results.size(); i++) {
        std::cout << "Blob " << i << ":" << std::endl;
        std::cout << "Area " << results[i].area() << std::endl;
        std::cout << "X center " << results[i].xCenter() << std::endl;
        std::cout << "Y center " << results[i].yCenter() << std::endl;
        std::cout << "Angle " << results[i].angle() << std::endl;
        std::cout << "Principal length 1 " << results[i].principalLength1() << std::endl;
        std::cout << "Principal length 2 " << results[i].principalLength2() << std::endl;
    }

    return 0;
}
