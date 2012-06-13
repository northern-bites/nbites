/**
 * @class ConnectTool
 * Version of QTool that just handles connection stuff.
 * Adds the following modules to the empty qtool:
 *
 * DataLoader
 * MemoryViewer
 * FieldViewer
 * OverseerClient
 *
 */

#pragma once

#include "EmptyQTool.h"

#include "data/DataLoader.h"
#include "viewer/MemoryViewer.h"
#include "viewer/FieldViewer.h"
#include "remote/RobotSelect.h"
#include "overseer/OverseerClient.h"

namespace qtool {

class ConnectTool : public EmptyQTool {

    Q_OBJECT

public:
    ConnectTool();
    ~ConnectTool();

private:
    data::DataLoader* dataLoader;
    viewer::MemoryViewer* memoryViewer;
    viewer::FieldViewer* fieldViewer;
    overseer::OverseerClient* overseerClient;
};

}
