#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QGridLayout>
#include <QFileDialog>
#include "YUVImage.h"
#include "RoboImageViewer.h"
#include "FileParser.h"
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
    RoboImage *roboImage;
    YUVImage *yuvImage;
    QLabel *shapeLabel;
    QLabel *infoLabel;
    QComboBox *shapeComboBox;
    memory::log::FileParser* fp;
    boost::shared_ptr<memory::proto::PImage> pImage;


};

#endif // WINDOW_H
