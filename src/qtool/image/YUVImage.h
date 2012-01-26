#ifndef YUVImage_H
#define YUVImage_H

/**
 * @class YUVImage
 *
 * Takes a robo image and splits it into 3 component images for each of the
 * three channels.
 * One instance of this class should rely on one instance of a roboImage.
 * Once the image data in the roboImage gets updated, make sure to update this
 * class as well by using updateFromRoboImage().
 *
 * This could be extended by adding in a listener to some data manager that will
 * push updates to the roboImage.
 */

#include <string>

#include <QGraphicsItem>
#include <QColor>
#include "ColorSpace.h"
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
    byte** getYImage() { return yImg;}
    byte** getUImage() { return uImg;}
    byte** getVImage() { return vImg;}

    bool areWithinImage(int x, int y) const {
        return 0 <= x && x < getWidth() && 0 <= y && y < getHeight();
    }

    //look up ternary operators - they're elegant in some cases
    //Warning - do not use these in any mass assignment or mass
    //access - you're much better off getting the image pointers
    //and using those - Octavian
    byte getY(int x, int y) const {
        return areWithinImage(x, y) ? yImg[x][y] : 0;
    }
    byte getU(int x, int y) const {
        return areWithinImage(x, y) ? uImg[x][y] : 0;
    }
    byte getV(int x, int y) const {
        return areWithinImage(x, y) ? vImg[x][y] : 0;
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
