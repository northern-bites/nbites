#pragma once

#include <vector>
#include <utility>

#include "Images.h"
#include "FuzzyLogic.h"

namespace man {
namespace vision {

class PostDetector {
public:
    PostDetector(const messages::PackedImage16& yImage, 
                 const messages::PackedImage8& whiteImage);
    ~PostDetector();

    int getLengthOfHistogram() const { return wd; }
    const messages::PackedImage8& getPostImage() const { return postImage; }
    double const* getUnfilteredHistogram() const { return unfilteredHistogram; }
    double const* getFilteredHistogram() const { return filteredHistogram; }
    const std::vector<int>& getCandidates() const { return candidates; }

private:
    void buildPostImage(const messages::PackedImage16& yImage, 
                        const messages::PackedImage8& whiteImage);
    Fool calculateGradScore(uint16_t* pt, int rowPitch, int pixelPitch);
    void buildHistogram();
    void filterHistogram();
    void convolve(double const* in, double const* kernel, int klength, double* out);
    void findPeaks();

    int wd;
    messages::PackedImage8 postImage;
    double* unfilteredHistogram;
    double* filteredHistogram;
    std::vector<int> candidates;

    static const int peakThreshold = 10;
};

}
}
