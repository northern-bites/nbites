#include "SharedTool.h"

namespace tool {

SharedTool::SharedTool(const char* title) :
    sharedView(this),
    EmptyTool(title)
{
    toolTabs->addTab(&sharedView, tr("SharedView"));
}

SharedTool::~SharedTool() {
}

} // namespace tool
