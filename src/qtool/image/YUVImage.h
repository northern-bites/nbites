#ifndef YUVImage_H
#define YUVImage_H

/**
 * @class YUVImage
 *
 * Takes a raw image YUV422 and splits it into 3 component images for each of the
 * three channels.
 * One instance of this class should rely on one instance of a rawImage.
 * Once the image data in the rawImage gets updated, make sure to update this
 * class as well by using the updateFromRawImage()
 *
 * Improvements : make this class actively "listen" for changes in the rawImage
 * (one could use signals and slots) and update the yuv image members accordingly
 *
 *
 * @author Octavian Neamtu
 */

#include <string>

#include <QGraphicsItem>
#include <QColor>
#include "man/memory/MImage.h"

class YUVImage
{

public:
    YUVImage(man::memory::MImage::const_ptr rawImage);
    virtual ~YUVImage();
    virtual void updateFromRawImage();
    void read(QString filename);
    void read(const byte* data);

    unsigned int getWidth() const { return width;}
    unsigned int getHeight() const { return height;}
    const byte** getYImage() const { return (const byte**) yImg;}
    const byte** getUImage() const { return (const byte**) uImg;}
    const byte** getVImage() const { return (const byte**) vImg;}

    bool areWithinImage(int x, int y) const {
        return 0 <= x && x < getWidth() && 0 <= y && y < getHeight();
    }

    //Warning - do not use these in any mass assignment or mass
    //access - you're much better off getting the image pointers
    //and using those - Octavian
    byte getY(int x, int y) const {
        assert(areWithinImage(x, y));
        return yImg[x][y];
    }
    byte getU(int x, int y) const {
        assert(areWithinImage(x, y));
        return uImg[x][y];
    }
    byte getV(int x, int y) const {
        assert(areWithinImage(x, y));
        return vImg[x][y];
    }

    bool rawImageDimensionsEnlarged();

private:
    void allocateYUVArrays(unsigned width, unsigned height);
    void deallocateYUVArrays();
    void resizeYUVArraysToFitRawImage();
    void resizeYUVArrays(unsigned width, unsigned height);


protected:
    man::memory::MImage::const_ptr rawImage;

    unsigned int width;
    unsigned int height;

    byte** yImg;
    byte** uImg;
    byte** vImg;
};
#endif // YUVImage_H
