/**
 * @class Viewer
 *
 * A class that maintains a UI that views the data
 * that the data manager has and is subsequently processed by
 * the Vision module. This can be used to work on vision
 * offline.
 *
 * @author Benjamin Mende
 */

#pragma once

#include <QWidget>
#include <QtGui>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "man/memory/Memory.h"
#include "man/vision/Vision.h"

#include "man/corpus/offlineconnect/OfflineImageTranscriber.h"
#include "image/ThresholdedImage.h"
#include "image/BMPYUVImage.h"
#include "image/VisualInfoImage.h"
#include "data/RobotMemoryManager.h"
#include "BMPImageViewerListener.h"
#include "ChannelImageViewer.h"
#include "MObjectViewer.h"
#include "CollapsibleImageViewer.h"

namespace qtool {
namespace viewer {

class VisionViewer : public QMainWindow {
    Q_OBJECT

public:
    VisionViewer(data::RobotMemoryManager::const_ptr memoryManager);

signals:
    void imagesUpdated();

public slots:
    void update();
    void loadColorTable();

    /* void setHorizonDebug(); */
    /* void setShootingDebug(); */
    /* void setOpenFieldDebug(); */
    /* void setEdgeDetectDebug(); */
    /* void setHoughDebug(); */
    /* void setRobotsDebug(); */
    /* void setBallDebug(); */
    /* void setBallDistDebug(); */
    /* void setCrossDebug(); */
    /* void setIdentCornersDebug(); */
    /* void setFieldEdgeDebug(); */

    /* void setPostPrintDebug(); */
    /* void setPostDebug(); */
    /* void setPostLogicDebug(); */
    /* void setPostCorrectDebug(); */
    /* void setPostSanityDebug(); */

    void setHorizonDebug(int state);
    void setShootingDebug(int state);
    void setOpenFieldDebug(int state);
    void setEdgeDetectionDebug(int state);
    void setHoughTransformDebug(int state);
    void setRobotsDebug(int state);
    void setVisualLinesDebug(int state);
    void setVisualCornersDebug(int state);
    void pixelClicked(int x, int y, int brushSize, bool leftClick);

private:
    std::vector<QDockWidget*> dockWidget;
    data::RobotMemoryManager::const_ptr memoryManager;

    image::ThresholdedImage* topVisionImage;
    image::ThresholdedImage* bottomVisionImage;
    BMPImageViewer* topVisionView;
    BMPImageViewer* bottomVisionView;

    boost::shared_ptr<Vision> vision;
    boost::shared_ptr<NaoPose> pose;
    boost::shared_ptr<Speech> speech;
    boost::shared_ptr<Sensors> sensors;
    man::memory::MVision::ptr offlineMVision;

    /* bool horizonD, shootD, openFieldD, edgeDetectD, houghD, robotsD; */
    /* bool ballD, ballDistD, identCornersD, dangerousBallD, fieldEdgeD; */
    /* bool crossD, postPrintD, postD, postLogicD, postCorrectD, postSanityD; */
    /* bool vertEdgeD, horEdgeD, secVertD, createLD, fitPointD, joinLD; */
    /* bool intersectLD, CcScanD, riskyCornerD, cornObjDistD; */

    MObjectViewer* offlineVisionView;

    boost::shared_ptr<man::memory::proto::PRawImage> topRawImage;
    boost::shared_ptr<man::memory::proto::PRawImage> bottomRawImage;
    man::corpus::OfflineImageTranscriber::ptr imageTranscribe;
};

}
}
