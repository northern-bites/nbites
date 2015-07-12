#pragma once

#include "EmptyTool.h"
#include "sharedview/SharedViewer.h"

namespace tool {

class SharedTool : public EmptyTool {
    Q_OBJECT;

public:
    SharedTool(const char* title = "SharedTool");
    ~SharedTool();

protected:
    sharer::SharedViewer sharedView;
};

} // namespace tool
