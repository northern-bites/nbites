
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

        //adds a next button to the logViewer
        QPushButton *nextButton = new QPushButton("Next");
        nextButton->setMaximumSize(50,80);
        nextButton->setGeometry(QRect(500,100,100,100));
        QHBoxLayout *layout= new QHBoxLayout();
        layout->addWidget(nextButton);
        QWidget *widget = new QWidget();
        widget->setLayout(layout);

        this->setMenuWidget(widget);
        connect (nextButton, SIGNAL(clicked()), this, SLOT(nextClicked()));

	std::vector<QTreeView> messageViewers;
	for (MObject_ID id = FIRST_OBJECT;
					id != LAST_OBJECT; id++) {
		if (id != MIMAGE_ID) {
			QDockWidget* dockWidget = new QDockWidget(QString(MObject::NameFromID(id).c_str()), this);
			MObjectViewer* view = new MObjectViewer(dataManager->getMemory()->getProtoMessage(id));
			dockWidget->setWidget(view);
                        dockWidget->setMinimumWidth(250);
			this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
			dataManager->addSubscriber(view);
			//view->
		}
	}

}

void LogViewer::nextClicked() {
    dataManager->getNext();
    roboImageViewer->repaint();
}

}
}
