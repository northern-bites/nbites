/*
 * @class ThresholdedImageDisplayModule
 * @class ImageDisplayModule
 * @class OverlayDisplayModule
 * @class ImageDisplayListener
 *
 * All of these classes subclass both QLabel, so that they can be added to a
 * GUI, and Module, so that they can be hooked up to one of our OutPortals and
 * display its image. Note that I chose QLabel because this is how Qt chooses
 * to display images: you get a QLabel and set its pixmap to be a QPixmap
 * that you've made. Hence QLabel is the simplest thing that you can actually
 * add to a GUI with an Image.
 *
 * ThresholdedImageDisplayModule: Displays a thresholded image (obviously).
 *    It takes as input a ThresholdedImage (@see Images.h) and can filter it
 *    by color, only showing one color or all colors, depending on what the
 *    user has requested via setFilter.
 *
 * ImageDisplayModule: Should more properly be called YUVImageDisplayModule,
 *     since it displays a YUVImage (@see Images.h). It can display this image
 *     in several channels. For example, if we want to only see the Y image
 *     for Hough transform stuff, set the channel type to Y. It converts the
 *     underlying image, with whatever channel filter was reqquested, to RGB
 *     pixels so that Qt can actually display it (side note: no one uses
 *     YUV422, so we have to convert this to a format that Qt can understand).
 *
 * OverlayDisplayModule: Inherits from ImageDisplayModule. Adds the ability to
 *     create a QImage and set it as the overlay for the YUV image.
 *
 * ImageDisplayListener: Inherits from ImageDisplayModule, simply to add
 *     mouse-listening methods. So if the user clicks on this module, it can
 *     register mouse clicks and do something with them.
 *
 * If you feel like digging through git, @see BMPImage.h, BMPImageViewer.h
 * and all of their descendants. These classes were heavily inspired by those,
 * aka I stole most of the code.
 *
 * @author Lizzie Mamantov
 * @date April 2013
 */

#pragma once

#include "RoboGrams.h"
#include "Images.h"
#include "Color.h"
#include <QLabel>
#include <QImage>
#include <QMouseEvent>

namespace tool {
namespace image {

// The different ways that we can display YUVImages
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

// Names for all of the different channels
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

// Side note:
// This could be a whole hell of a lot more elegant
// But I am hacking as fast as I can
// So future person who is reading this code, please don't judge me
// --Lizzie

/*
 * THRESHOLDED IMAGE DISPLAY
 */
class ThresholdedImageDisplayModule : public QLabel,
                                      public portals::Module
{
    Q_OBJECT;

public:
    ThresholdedImageDisplayModule(QWidget* parent = 0);

    portals::InPortal<messages::ThresholdImage> imageIn;

    // The filter tells us which color to show; a class that uses this one
    // to display an image can change the filter using this method
    void setFilter(byte filter_) { filter = filter; }

protected:
    // Implements Module's run_ method
    virtual void run_();

    // Take the image we have on the inPortal and turn it into a QImage
    // that Qt can understand
    QImage makeImage(byte filter);

    /// Which color are we displaying?
    byte filter;
};

/*
 * YUV IMAGE DISPLAY
 */

class ImageDisplayModule : public QLabel,
                           public portals::Module
{
    Q_OBJECT;

public:
    ImageDisplayModule(QWidget* parent = 0);

    portals::InPortal<messages::YUVImage> imageIn;

    // So that a display using this can figure out which channel this is
    ChannelType getChannelType() { return channel; }
    std::string getChannelLabel() { return ChannelType_label[channel]; }

protected:
    // Implements Module's run_ method
    virtual void run_();

    // Take the iamge from the InPortal and make a QImage, which can actually
    // be displayed by Qt, taking the desired channel into account
    QImage makeImageOfChannel(ChannelType channel);

    // Which channel are we currently displaying?
    ChannelType channel;
};

/*
 * OVERLAY DISPLAY
 */

class OverlayDisplayModule : public ImageDisplayModule
{
public:
    OverlayDisplayModule(QWidget* parent = 0) : ImageDisplayModule(parent) {};
    void setOverlay(QImage overlay_) { overlay = overlay_; }

protected:
    // Replaces ImageDisplayModule's run method to do the overlaying
    virtual void run_();

    QImage overlay;
};

/*
 * YUV IMAGE DISPLAY WITH MOUSE LISTENING
 */

class ImageDisplayListener: public ImageDisplayModule {
    Q_OBJECT;

    static const int DEFAULT_BRUSH_SIZE = 10;

public:
    ImageDisplayListener(QWidget *parent = 0);

    // Mouse methods stolen directly from older class--BMPImageViewerListener
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
