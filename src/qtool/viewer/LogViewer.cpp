
#include "LogViewer.h"
#include <vector>
#include "man/memory/Memory.h"

namespace qtool {
namespace viewer {

using namespace data;
using namespace man::memory;

LogViewer::LogViewer(DataManager::ptr dataManager) :
		dataManager(dataManager),
		roboImageViewer(dataManager->getMemory()->getRoboImage().get()){

	QVBoxLayout *layout = new QVBoxLayout;
	layout->setAlignment(Qt::AlignTop);

	std::vector<QTreeView> messageViewers;
	for (MObject_ID id = FIRST_OBJECT;
					id != LAST_OBJECT; id++) {
		if (id == MIMAGE_ID) {
			connect(dataManager.get(), SIGNAL(newDataEvent(DataEvent)),
					roboImageViewer, SLOT(newDataEvent(DataEvent)));
		}
	}
	layout->addWidget(&roboImageViewer);
	this->setLayout(layout);

}

}
}
