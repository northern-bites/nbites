#pragma once

#include "EmptyTool.h"

#include "worldview/WorldView.h"

namespace tool {

class WorldviewTool : public EmptyTool {
    Q_OBJECT;

public:
    WorldviewTool(const char* title = "WorldviewTool");
    ~WorldviewTool();

protected:
    worldview::WorldView worldView;
};
}
