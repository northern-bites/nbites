/**
 * @class OfflineTool
 *
 * A TOOL for running man offline 
 *
 * @author Octavian Neamtu
 */

#pragma once

#include "EmptyQTool.h"

namespace qtool {

class OfflineTool : public EmptyQTool {

    Q_OBJECT

public:
    OfflineTool();
    virtual ~OfflineTool();
};

}
