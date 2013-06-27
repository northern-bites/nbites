#pragma once

#include "EmptyTool.h"

#include "ballview/BallViewer.h"

namespace tool {

class BallTool : public EmptyTool {
    Q_OBJECT;

public:
    BallTool(const char* title = "BallTool");
    ~BallTool();

public slots:
    void setUpModules();

protected:
    ballview::BallViewer ballView;
};

} // namespace tool
