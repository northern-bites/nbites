/**
 * @class ColorTableCreator
 *
 * Tool to define/calibrate a color table
 *
 * @author EJ Googins
 * @withsomehelpfrom Octavian Neamtu
 */

#pragma once

#include <QWidget>
#include <QPushButton>
#include <QComboBox>

// colorcreator
#include "ColorTable.h"

#include "data/DataManager.h"
#include "image/BMPYUVImage.h"
#include "corpus/alconnect/ALConstants.h"
#include "corpus/ColorParams.h"
#include "corpus/ImageAcquisition.h"
//qtool
#include "viewer/BMPImageViewerListener.h"
#include "ColorEdit.h"
#include "ColorSpace.h"
#include "ColorSpaceWidget.h"
#include "ColorWheel.h"
#include "image/ThresholdedImage.h"

#include "man/corpus/offlineconnect/OfflineImageTranscriber.h"

namespace qtool {
namespace colorcreator {

/**
 * @class BrushStroke
 *
 * Saves a color paint action (like defining or undefining a color on a region in the image)
 *
 */
class BrushStroke {

public:
    BrushStroke(int x, int y, image::ColorID color, int brushSize, bool define)
        : x(x), y(y), color(color), brushSize(brushSize), define(define) { }

    BrushStroke invert() const { return BrushStroke(x, y, color, brushSize, !define); }

    int x, y;
    image::ColorID color;
    int brushSize;
    bool define;

};

class ColorTableCreator: public QWidget {

    Q_OBJECT

public:
    static const image::ColorID STARTING_COLOR = image::Orange;

public:
    ColorTableCreator(qtool::data::DataManager::ptr dataManager,
            QWidget *parent = 0);
    ~ColorTableCreator() {    }

    void paintMeLikeOneOfYourFrenchGirls(const BrushStroke& brushStroke);


protected slots:
    void loadColorTable();
    void saveColorTable();
    void updateThresholdedImage();
    void canvassClicked(int x, int y, int brushSize, bool leftClick);
    void undo();
    void updateColorSelection(int color);
    void updateColorStats();
    void imageTabSwitched(int);

private:
    data::DataManager::ptr dataManager;

    QTabWidget* imageTabs;

    man::corpus::Camera::Type currentCamera;

    image::BMPYUVImage* topImage;
    viewer::BMPImageViewerListener* topImageViewer;

    image::BMPYUVImage* bottomImage;
    viewer::BMPImageViewerListener* bottomImageViewer;

    boost::shared_ptr<Sensors> sensors;
    man::corpus::OfflineImageTranscriber::ptr imageTranscribe;
    boost::shared_ptr<man::memory::proto::PRawImage> rawThresholdedImageData; // Octavians idea for name
    image::ThresholdedImage* threshImage;
    viewer::BMPImageViewer* thresholdedImageViewer;

    QLabel* colorStats;

    ColorTable colorTable;

    QComboBox colorSelect;
    int currentColor;

    std::vector<BrushStroke> brushStrokes;

};

class FixedLayout: public QVBoxLayout{
	Q_OBJECT

public:
	QSize sizeHint() const {return QSize(350,500);}
};


}
}
