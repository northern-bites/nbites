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
    void read(std::string s);
    unsigned int getWidth() const { return width;}
    unsigned int getHeight() const { return height;}
    int** getYImage() const { return yImg;}
    int** getUImage() const { return uImg;}
    int** getVImage() const { return vImg;}
    bool areWithinImage(int x, int y) const;
    int getY(int x, int y) const;
    int getU(int x, int y) const;
    int getV(int x, int y) const;
    int getRed(int x, int y) const;
    int getGreen(int x, int y) const;
    int getBlue(int x, int y) const;
    int getH(int x, int y) const;
    int getS(int x, int y) const;
    int getZ(int x, int y) const;

protected:
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

    int** yImg;
    int** uImg;
    int** vImg;
};
#endif // YUVImage_H
