/**
 * Renders a BMPYUVImage (that's usually a robot image)
 * with a combo box for selecting different render channels
 * on the image
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include <QComboBox>
#include <QMouseEvent>
#include "BMPImageViewer.h"
#include "BMPImageViewerListener.h"
#include "image/BMPYUVImage.h"

namespace qtool {
namespace viewer {

// inherit from BMPImageViewerListener so we can get mouse clicks
class ChannelImageViewer: public BMPImageViewerListener {
    Q_OBJECT;
public:

    ChannelImageViewer(image::BMPYUVImage* image,
                QWidget *parent = NULL);

    virtual ~ChannelImageViewer() {}

    virtual QSize minimumSizeHint() {
        //make room for the combo box
        return BMPImageViewer::minimumSizeHint() +
                   QSize(0, channelSelect.height());
    }


public slots:
    void selectionChanged(int i);

protected:
    void setupUI();

protected:
    image::BMPYUVImage* bmpyuvimage;
    QComboBox channelSelect;



};

}
}
