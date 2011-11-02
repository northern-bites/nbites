
#include "OfflineViewer.h"

using namespace man::corpus;
using namespace man::memory;
using namespace qtool::viewer;

namespace qtool {
namespace offline {

OfflineViewer::OfflineViewer(Memory::const_ptr memory, QWidget* parent) : QWidget(parent),
        offlineControl(new OfflineManController(memory)),
        manPreloader(offlineControl),
        manMemoryViewer(new MemoryViewer(offlineControl->getManMemory())) {

    memory->addSubscriber(this, MIMAGE_ID);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(manMemoryViewer);

    this->setLayout(layout);
}

void OfflineViewer::update(data::MObject_ID id) {
    offlineControl->signalNextImageFrame();
}

}
}
