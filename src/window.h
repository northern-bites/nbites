#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QGridLayout>
#include <QFileDialog>

#include <boost/shared_ptr.hpp>

#include "YUVImage.h"
#include "RoboImageViewer.h"
#include "ImageParser.h"
#include "memory/protos/Sensors.pb.h"

class QCheckBox;
class QComboBox;
class QLabel;
class QSpinBox;
class RenderArea;

class Window : public QWidget
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = 0);

private:
    RoboImageViewer *roboImageViewer;
    boost::shared_ptr<RoboImage> roboImage;
    QLabel *shapeLabel;
    QLabel *infoLabel;
    QComboBox *shapeComboBox;
    memory::log::ImageParser* imageParser;

};

#endif // WINDOW_H
