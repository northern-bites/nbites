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
#include "data/MemorySignalingInterface.h"

#include "OverseerDef.h"

namespace qtool {
namespace overseer {

class OverseerClient : public QWidget {

    Q_OBJECT;

    typedef man::memory::parse::MessageParser MessageParser;

public:
    OverseerClient(QWidget* parent = 0);
    ~OverseerClient() {}

public slots:
    void connectToOverseer();

protected:
    GroundTruth::ptr groundTruth;
    MessageParser::ptr messageParser;
    viewer::MObjectViewer groundTruthView;
    QPushButton* connectButton;

};

}
}
