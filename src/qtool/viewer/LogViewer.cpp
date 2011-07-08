
#include "LogViewer.h"
#include <vector>
#include "man/memory/Memory.h"

namespace qtool {
namespace viewer {

using namespace data;
using namespace man::memory;

LogViewer::LogViewer(DataManager::ptr dataManager) :
		dataManager(dataManager),
		roboImageViewer(new RoboImageViewer(
				dataManager->getMemory()->getRoboImage())){

	QVBoxLayout *layout = new QVBoxLayout;
	layout->setAlignment(Qt::AlignTop);

	dataManager->addSubscriber(roboImageViewer, NEW_IMAGE);

	std::vector<QTreeView> messageViewers;
	for (MObject_ID id = FIRST_OBJECT;
					id != LAST_OBJECT; id++) {
		if (id != MIMAGE_ID) {
			data::treemodel::ProtoNode* root =
					new data::treemodel::ProtoNode(NULL, NULL,
							dataManager->getMemory()->getProtoMessage(id).get());
			data::treemodel::TreeModel* messageModel = new data::treemodel::TreeModel(root);
			QTreeView* view = new QTreeView;
			view->setModel(messageModel);
			layout->addWidget(view);
		}
	}
	layout->addWidget(roboImageViewer);
	this->setLayout(layout);

}

}
}
