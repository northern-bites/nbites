#include "MemoryViewer.h"

namespace tool {
namespace viewer {

MemoryViewer::MemoryViewer(QWidget* parent) : QMainWindow(parent)
{
	QHBoxLayout* mainLayout = new QHBoxLayout;
	QWidget* mainWidget = new QWidget;
    // We don't need this right now but...
    QTabWidget* imageTabs = new QTabWidget();

	mainLayout->addWidget(imageTabs);
	mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	mainWidget->setLayout(mainLayout);
    this->setCentralWidget(mainWidget);

    //corner ownership
    this->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    std::vector<QTreeView> messageViewers;
    for (Memory::const_iterator iterator = memory->begin(); iterator != memory->end(); iterator++) {
        QDockWidget* dockWidget = new QDockWidget(QString(iterator->first.c_str()), this);
        dockWidget->setMinimumWidth(300);
        dockWidget->setMaximumHeight(125);
        MObjectViewer* view = new MObjectViewer(iterator->second, dockWidget);
        dockWidget->setWidget(view);
        this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
        memoryManager->connectSlot(view, SLOT(updateView()), iterator->first);
    }

}
}
}
