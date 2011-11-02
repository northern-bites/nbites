
#include "MemoryViewer.h"
#include <vector>

namespace qtool {
namespace viewer {

using namespace data;
using namespace man::memory;

    MemoryViewer::MemoryViewer(Memory::const_ptr memory) :
         memory(memory),
         roboImageViewer(new RoboImageViewer(memory->getMImage())) {

        toolbar = new QToolBar();
        nextButton = new QPushButton(tr("&Next"));
        prevButton = new QPushButton(tr("&Previous"));

        connect(nextButton, SIGNAL(clicked()), this, SLOT(nextImage()));
        connect(prevButton, SIGNAL(clicked()), this, SLOT(prevImage()));

        toolbar->addWidget(prevButton);
        toolbar->addWidget(nextButton);

        this->addToolBar(toolbar);

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

    void MemoryViewer::nextImage() {
    }

    void MemoryViewer::prevImage() {
    }

}
}
