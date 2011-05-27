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

class YUVImage
{
public:
    enum DisplayModes
    {
        Color,
        Y,
        U,
        V,
        Red,
        Green,
        Blue,
        Hue,
        Saturation,
        Value
    } display;

public:
    YUVImage(int wd, int ht);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void read(QString filename);
    void read(std::string s);
    int getWidth() {return width;}
    int getHeight(){return height;}
    int** getYImage() { return yImg;}
    int** getUImage() { return uImg;}
    int** getVImage() { return vImg;}
    int getY(int x, int y) { return yImg[x][y];}
    int getU(int x, int y) { return uImg[x][y];}
    int getV(int x, int y) { return vImg[x][y];}
    int getRed(int x, int y);
    int getGreen(int x, int y);
    int getBlue(int x, int y);
    int getH(int x, int y);
    int getS(int x, int y);
    int getZ(int x, int y);
    QImage bmp();


private:
    int width;
    int height;
    int** yImg;
    int** uImg;
    int** vImg;
};

#endif // YUVImage_H
