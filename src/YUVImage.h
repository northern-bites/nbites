#ifndef YUVImage_H
#define YUVImage_H

#include <string>

#include <QGraphicsItem>
#include <QString>
#include <QImage>
#include <QColor>
#include <QDataStream>
#include <QTextStream>
#include <QFile>
#include <QPainter>
#include "colorspace.h"
#include "RoboImage.h"

class YUVImage
{

public:
    YUVImage(const RoboImage* _roboImage);
    virtual ~YUVImage();
    QRectF boundingRect() const;
    void updateFromRoboImage();
    void read(QString filename);
    void read(std::string s);
    unsigned int getWidth() const {return width;}
    unsigned int getHeight() const {return height;}
    int** getYImage() const { return yImg;}
    int** getUImage() const { return uImg;}
    int** getVImage() const { return vImg;}
    int getY(int x, int y) const { return yImg[x][y];}
    int getU(int x, int y) const { return uImg[x][y];}
    int getV(int x, int y) const { return vImg[x][y];}
    int getRed(int x, int y) const;
    int getGreen(int x, int y) const;
    int getBlue(int x, int y) const;
    QImage bmp();


private:
    const RoboImage* roboImage;

    unsigned int width;
    unsigned int height;

    int** yImg;
    int** uImg;
    int** vImg;
};

#endif // YUVImage_H
