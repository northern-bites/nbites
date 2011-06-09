#ifndef OldRoboImage_H
#define OldRoboImage_H

#include <QGraphicsItem>
#include <QString>
#include <QImage>
#include <QColor>
#include <QDataStream>
#include <QTextStream>
#include <QFile>
#include <QPainter>
#include "Image/ColorSpace.h"
#include "colorzone.h"

class OldRoboImage
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
    OldRoboImage(int wd, int ht);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void read(QString filename);
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
    int** monoChrome() { return yImg;}
    QImage fast();
    QImage bmp();
    void yuv(int i, int j, int y, int u, int v);
    void rgb(int i, int j, int r, int g, int b);


private:
    int width;
    int height;
    int** yImg;
    int** uImg;
    int** vImg;
};

#endif // OldRoboImage_H
