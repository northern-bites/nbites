
#include "MemoryViewer.h"
#include <vector>

namespace qtool {
namespace viewer {

using namespace data;
using namespace man::memory;

    MemoryViewer::MemoryViewer(Memory::const_ptr memory) :
         memory(memory),
         roboImageViewer(new RoboImageViewer(memory->getMImage())) {

        this->setCentralWidget(roboImageViewer);
        memory->addSubscriber(roboImageViewer, MIMAGE_ID);

        std::vector<QTreeView> messageViewers;
        for (MObject_ID id = FIRST_OBJECT;
                id != LAST_OBJECT; id++) {
            if (id != MIMAGE_ID) {
                QDockWidget* dockWidget =
                        new QDockWidget(QString(MObject_names[id].c_str()), this);
                MObjectViewer* view = new MObjectViewer(memory->
                                                        getMObject(id)->getProtoMessage());
                dockWidget->setWidget(view);
                this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
                memory->addSubscriber(view, id);
            }
        }

    }
}
}
