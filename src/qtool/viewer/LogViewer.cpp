
#include "LogViewer.h"
#include <vector>
#include "man/memory/Memory.h"

namespace qtool {
namespace viewer {

using namespace data;
using namespace man::memory;

LogViewer::LogViewer(DataManager::ptr dataManager) :
		QMainWindow(),
		dataManager(dataManager),
		roboImageViewer(new RoboImageViewer(
				dataManager->getMemory()->getRoboImage())){

	this->setCentralWidget(roboImageViewer);
	dataManager->addSubscriber(roboImageViewer, MIMAGE_ID);

	std::vector<QTreeView> messageViewers;
	for (MObject_ID id = FIRST_OBJECT;
					id != LAST_OBJECT; id++) {
		if (id != MIMAGE_ID) {
			QDockWidget* dockWidget = new QDockWidget(QString(MObject::NameFromID(id).c_str()), this);
			MObjectViewer* view = new MObjectViewer(dataManager->getMemory()->getProtoMessage(id));
			dockWidget->setWidget(view);
			this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
			dataManager->addSubscriber(view);
			//view->
		}
	}

}

}
}
