#ifndef EdgeDetector_h_DEFINED
#define EdgeDetector_h_DEFINED



/**
 * Image channel gradient information struct
 */
typedef struct gradient_t {
    int x[IMAGE_HEIGHT][IMAGE_WIDTH];
    int y[IMAGE_HEIGHT][IMAGE_WIDTH];
    int mag[IMAGE_HEIGHT][IMAGE_WIDTH];
    int rows, cols;
    bool[IMAGE_HEIGHT][IMAGE_WIDTH] peaks;
} Gradient;


class EdgeDetector
{
public:
    EdgeDetector(int thresh);
    virtual ~EdgeDetector();

/**
 * Public interface
 */
public:
    void detectEdges(const uchar* channel,
                     const int startOffset,
                     const int chanOffset,
                     Gradient& gradient);

    int  getThreshold()           { return threshold; }
    void setThreshold(int thresh) { threshold = thresh; }

private:
    void sobelOperator(const uchar* channel, const int offset,
                       Gradient& gradient);
    void findPeaks(Gradient& gradient);
    byte dir(int y, int x);

private:
    // Tables that specify the + neighbor of a pixel indexed by
    // gradient direction octant (the high 3 bits of direction).
    const static int DIRECTIONS = 8;
    const static int dxTab[DIRECTIONS] = { 1,  1,  0, -1, -1, -1,  0,  1};
    const static int dyTab[DIRECTIONS] = { 0,  1,  1,  1,  0, -1, -1, -1};

    int threshold;

};

#endif /* EdgeDetector_h_DEFINED */
