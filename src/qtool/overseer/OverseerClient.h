/**
 * @class OverseerClient
 *
 * Widget to manage a client to the overseer ground truth server
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <QWidget>
#include <QPushButton>

#include "GroundTruth.h"
#include "man/memory/parse/MessageParser.h"
#include "viewer/MObjectViewer.h"

#include "viewer/BMPImageViewer.h"
#include "image/PaintField.h"
#include "image/PaintGroundTruth.h"
#include "data/DataManager.h"

#include "OverseerDef.h"

namespace qtool {
namespace overseer {

class OverseerClient : public QWidget {

    Q_OBJECT;

    typedef man::memory::parse::MessageParser MessageParser;

public:
    OverseerClient(data::DataManager::ptr dataManager, QWidget* parent = 0);
    ~OverseerClient() {}

public slots:
    void connectToOverseer();
    void newGroundTruth();

protected:
    data::DataManager::ptr dataManager;
    GroundTruth::const_ptr groundTruth;
    MessageParser::ptr messageParser;
    QPushButton* connectButton;
    QLabel* fpsLabel;
    int64_t last_timestamp;

};

}
}
