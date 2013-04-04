#include "LogViewer.h"

namespace tool {
namespace logview {

LogViewer::LogViewer(QWidget* parent) : QMainWindow(parent),
                                        imageTabs(this)
{
	QHBoxLayout* mainLayout = new QHBoxLayout;
	QWidget* mainWidget = new QWidget;

	mainLayout->addWidget(&imageTabs);
	mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	mainWidget->setLayout(mainLayout);
    this->setCentralWidget(mainWidget);

    //corner ownership
    this->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
}

}
}
