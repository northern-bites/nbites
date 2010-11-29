#ifndef _ImageAcquisitionTest_h_DEFINED
#define _ImageAcquisitionTest_h_DEFINED

#include "Tests.h"
#include "ColorParams.h"
#include "VisionDef.h"

typedef unsigned char uchar;

class ImageAcquisitionTest
{
public:
    ImageAcquisitionTest();
    virtual ~ImageAcquisitionTest();

    int runTests();

private:
    void test_color_segmentation();
    void test_avg();

    void init();
    void setup(int y0, int u0, int v0,
               int y1, int u1, int v1,
               int yn, int un, int vn);
    void allocate();
    void deallocate();


    // Helper functions
    int yZero()  const { return static_cast<int>(c.yZero & 0xFFFF);  }
    int ySlope() const { return static_cast<int>(c.ySlope & 0xFFFF); }
    int yLimit() const { return static_cast<int>(c.yLimit & 0xFFFF); }

    int uZero()  const { return static_cast<int>(c.uvZero & 0xFFFF);  }
    int uSlope() const { return static_cast<int>(c.uvSlope & 0xFFFF); }
    int uLimit() const { return static_cast<int>(c.uvLimit & 0xFFFF); }

    int vZero()  const { return static_cast<int>((c.uvZero >> 16) & 0xFFFF);  }
    int vSlope() const { return static_cast<int>((c.uvSlope >> 16) & 0xFFFF); }
    int vLimit() const { return static_cast<int>((c.uvLimit >> 16) & 0xFFFF); }

    int yAvgValue(int i, int j) const;
    int uAvgValue(int i, int j) const;
    int vAvgValue(int i, int j) const;

    int yIndex(int i, int j) const;
    int uIndex(int i, int j) const;
    int vIndex(int i, int j) const;

    int tableLookup(int y, int u, int v) const;
    int colorValue(int i, int j) const;

    enum {
        tableMaxSize = 128 * 128 * 128,
    };

    uchar * out, *yuv,* yuvCopy,* table;
    ColorParams c;
};

#endif /* _ImageAcquisitionTest_h_DEFINED */
