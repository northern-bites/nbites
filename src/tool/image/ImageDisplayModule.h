#pragma once

#include "RoboGrams.h"
#include "Images.h"
#include "Color.h"
#include <QLabel>
#include <QImage>
#include <QMouseEvent>

namespace tool {
namespace image {

enum ChannelType {
    RGB,
    Y,
    U,
    V,
    Red,
    Green,
    Blue,
    Hue,
    Saturation,
    Value,
    NUM_CHANNELS
};

static const std::string ChannelType_label[] = {
    "RGB",
    "Y",
    "U",
    "V",
    "Red",
    "Green",
    "Blue",
    "Hue",
    "Saturation",
    "Value"
};

// This could be a whole hell of a lot more elegant
// But I am hacking as fast as I can
// So future person who is reading this code, please don't judge me
// --Lizzie
class ThresholdedImageDisplayModule : public QLabel,
                                      public portals::Module
{
    Q_OBJECT;

public:
    ThresholdedImageDisplayModule(QWidget* parent = 0);

    portals::InPortal<messages::ThresholdImage> imageIn;

    void setFilter(byte filter_) { filter = filter; }

protected:
    virtual void run_();

    QImage makeImage(byte filter);

    byte filter;
};

class ImageDisplayModule : public QLabel,
                           public portals::Module
{
    Q_OBJECT;

public:
    ImageDisplayModule(QWidget* parent = 0);

    portals::InPortal<messages::YUVImage> imageIn;

    ChannelType getChannelType() { return channel; }
    std::string getChannelLabel() { return ChannelType_label[channel]; }

protected:
    virtual void run_();

    QImage makeImageOfChannel(ChannelType channel);

    ChannelType channel;
};

class ImageDisplayListener: public ImageDisplayModule {
    Q_OBJECT;

    static const int DEFAULT_BRUSH_SIZE = 10;

public:
    ImageDisplayListener(QWidget *parent = 0);

    void mouseReleaseEvent ( QMouseEvent *event);
    // Scroll up or down to increase or decrease brush size
    void wheelEvent(QWheelEvent* event);
    void setBrushColor(QColor _brushColor) { brushColor = _brushColor; updateBrushCursor();}
    int getBrushSize() { return brushSize; }

signals:
    void mouseClicked(int x, int y, int brushSize, bool leftClick);

private:
    void updateBrushCursor();

private:
    QColor brushColor;
    int brushSize;
};


}
}
