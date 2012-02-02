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
#include "BMPImageViewer.h"
#include "image/BMPYUVImage.h"

namespace qtool {
namespace viewer {

class ChannelImageViewer: public BMPImageViewer {
    Q_OBJECT;
public:

    ChannelImageViewer(image::BMPYUVImage::ptr image,
                QWidget *parent = NULL);
    ChannelImageViewer(man::memory::MImage::const_ptr mImage,
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
    image::BMPYUVImage::ptr bmpyuvimage;
    QComboBox channelSelect;



};

}
}
