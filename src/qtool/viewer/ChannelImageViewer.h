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
