#ifndef EdgeDetector_h_DEFINED
#define EdgeDetector_h_DEFINED

#include "VisionDef.h"

/**
 * Image channel gradient information struct
 */
typedef struct gradient_t {
    int x[IMAGE_HEIGHT][IMAGE_WIDTH];
    int y[IMAGE_HEIGHT][IMAGE_WIDTH];
    int mag[IMAGE_HEIGHT][IMAGE_WIDTH];
    int rows, cols;
    bool peaks[IMAGE_HEIGHT][IMAGE_WIDTH];
} Gradient;

typedef struct channel_t {
    int val[IMAGE_HEIGHT][IMAGE_WIDTH];
} Channel;


class EdgeDetector
{
public:
    EdgeDetector(int thresh);
    virtual ~EdgeDetector(){ };

/**
 * Public interface
 */
public:
    void detectEdges(const Channel& channel,
                     Gradient& gradient);

    int  getThreshold()           { return threshold; }
    void setThreshold(int thresh) { threshold = thresh; }

private:
    void sobelOperator(const Channel& channel,
                       Gradient& gradient);
    void findPeaks(Gradient& gradient);
    int dir(int y, int x);

private:
    // Tables that specify the + neighbor of a pixel indexed by
    // gradient direction octant (the high 3 bits of direction).
    const static int DIRECTIONS = 8;
    const static int dxTab[DIRECTIONS];
    const static int dyTab[DIRECTIONS];

    int threshold;
};

#endif /* EdgeDetector_h_DEFINED */
