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

    int getLengthOfHistogram() const { return len; }
#ifdef OFFLINE
    const messages::PackedImage8& getPostImage() const { return postImage; }
#endif
    double const* getUnfilteredHistogram() const { return unfilteredHistogram; }
    double const* getFilteredHistogram() const { return filteredHistogram; }
    const std::vector<int>& getCandidates() const { return candidates; }

private:
    inline Fool calculateGradScore(int16_t magnitude, int16_t gradX, int16_t gradY) const;
#ifdef OFFLINE
    void buildPostImage(const Gradient& gradient,
                        const messages::PackedImage8& whiteImage);
#endif
    void buildHistogram(const Gradient& gradient,
                        const messages::PackedImage8& whiteImage);
    void filterHistogram();
    void convolve(double const* in, double const* kernel, int klength, double* out);
    void findPeaks();

    int len;
#ifdef OFFLINE
    messages::PackedImage8 postImage;
#endif
    double* unfilteredHistogram;
    double* filteredHistogram;
    std::vector<int> candidates;

    static const int peakThreshold = 30;
};

}
}
