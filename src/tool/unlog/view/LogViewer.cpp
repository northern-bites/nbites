#include "LogViewer.h"

namespace tool {
namespace unlog {
namespace view {

LogViewer::LogViewer(QWidget* parent) : QMainWindow(parent)
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

}

void LogViewer::addProtoViewer(UnlogBase* unlogger)
{
    unlogger->useGUI(true);
    QDockWidget* dockWidget =
        new QDockWidget(QString(unlogger->getType().c_str()),
                                this);
    dockWidget->setMinimumWidth(300);
    dockWidget->setMaximumHeight(125);
    ProtoViewer* viewer = new ProtoViewer(unlogger->getMessage(), dockWidget);

    QObject::connect(unlogger, SIGNAL(newMessage(const google::protobuf::Message*)),
                     viewer, SLOT(updateView(const google::protobuf::Message*)));

    dockWidget->setWidget(viewer);
    this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
}

}
}
}
