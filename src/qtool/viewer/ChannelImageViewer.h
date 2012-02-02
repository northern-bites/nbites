/**
 * Renders a BMPYUVImage with channel selection
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
    virtual ~ChannelImageViewer() {}

    virtual QSize minimumSizeHint() {
        //make room for the combo box
        return BMPImageViewer::minimumSizeHint() +
                   QSize(0, channelSelect.height());
    }

public slots:
    void selectionChanged(int i);

protected:
    image::BMPYUVImage::ptr bmpyuvimage;
    QComboBox channelSelect;



};

}
}
