/**
 * @class QTool
 * Main QTool Class
 * Adds the following modules to the empty qtool (ALL OF THEM):
 *
 * DataLoader
 * ColorCalibrate
 * ColorTableCreator
 * MemoryViewer
 * VisionViewer
 * OfflineViewer
 * BallEKFViewer
 * FieldViewer
 * OverseerClient
 *
 * @author Octavian Neamtu
 */

#pragma once

#include "EmptyQTool.h"

#include "data/DataLoader.h"
#include "colorcreator/ColorCalibrate.h"
#include "colorcreator/ColorTableCreator.h"
#include "viewer/MemoryViewer.h"
#include "viewer/VisionViewer.h"
#include "offline/OfflineViewer.h"
#include "viewer/BallEKFViewer.h"
#include "viewer/ParticleViewer.h"
#include "remote/RobotSelect.h"
#include "overseer/OverseerClient.h"
//#include "viewer/GraphViewer.h"

namespace qtool {

class QTool : public EmptyQTool {

    Q_OBJECT

public:
    QTool();
    ~QTool();

private:
    data::DataLoader* dataLoader;
    colorcreator::ColorCalibrate* colorCalibrate;
    colorcreator::ColorTableCreator* colorTableCreator;
    viewer::MemoryViewer* memoryViewer;
    viewer::VisionViewer* visionViewer;
    offline::OfflineViewer* offlineViewer;
    viewer::BallEKFViewer* ballEKFViewer;
    viewer::ParticleViewer* fieldViewer;
    overseer::OverseerClient* overseerClient;
    //viewer::GraphViewer* graphViewer;

    QPushButton* prevButton;
    QPushButton* nextButton;
    QPushButton* recordButton;
    QToolBar* toolbar;
};

}
