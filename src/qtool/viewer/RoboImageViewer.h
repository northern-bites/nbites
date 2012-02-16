/**
 * @class RoboImageViewer
 *
 * Takes an MImage and draws in in a widget
 *
 * It does this using the raw YUY2 (YUYV) data and ffmpeg swscale library
 *
 * see
 * http://dranger.com/ffmpeg/tutorial02.html
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QLayout>

#include "data/DataTypes.h"

namespace qtool {
namespace viewer {

class RoboImageViewer : public QWidget {

    Q_OBJECT;

public :
    RoboImageViewer(data::RoboImage::const_ptr rawImage, QWidget *parent);
    virtual ~RoboImageViewer();

    virtual QSize minimumSizeHint() const {
        return QSize(roboImage->get()->width(), roboImage->get()->height());
    }

public slots:
    void updateView();

private:
    data::RoboImage::const_ptr roboImage;
    QLabel imagePlaceholder;
};

}
}
