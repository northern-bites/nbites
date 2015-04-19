#pragma once

#include <vector>
#include <utility>

#include "Images.h"
#include "FuzzyLogic.h"
#include "Gradient.h"

namespace man {
namespace vision {

class PostDetector {
public:
    PostDetector(const Gradient& gradient,
                 const messages::PackedImage8& whiteImage);
    ~PostDetector();

    int getLengthOfHistogram() const { return wd; }
    const messages::PackedImage8& getPostImage() const { return postImage; }
    double const* getUnfilteredHistogram() const { return unfilteredHistogram; }
    double const* getFilteredHistogram() const { return filteredHistogram; }
    const std::vector<int>& getCandidates() const { return candidates; }

private:
    void buildPostImage(const Gradient& gradient,
                        const messages::PackedImage8& whiteImage);
    inline Fool calculateGradScore(int16_t gradX, int16_t gradY) const;
    void applyMathMorphology();
    void buildHistogram();
    void filterHistogram();
    void convolve(double const* in, double const* kernel, int klength, double* out);
    void findPeaks();

    int wd;
    messages::PackedImage8 postImage;
    double* unfilteredHistogram;
    double* filteredHistogram;
    std::vector<int> candidates;

    static const int peakThreshold = 30;
};

}
}
