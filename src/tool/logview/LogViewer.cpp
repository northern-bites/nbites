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

void LogViewer::newDisplayWidget(QWidget* widget, std::string title)
{
    if (title != "Top Image" && title != "Bottom Image")
    {
        QDockWidget* dockWidget =
            new QDockWidget(QString(title.data()), this);
        dockWidget->setMinimumWidth(300);
        dockWidget->setMaximumHeight(800);

        dockWidget->setWidget(widget);
        dockWidgets.push_back(dockWidget);
        this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
    }
    else
    {
        imageTabs.addTab(widget, QString(title.data()));
    }

}

void LogViewer::deleteDisplayWidgets()
{
    for (std::vector<QDockWidget*>::iterator i = dockWidgets.begin();
         i != dockWidgets.end(); i++)
    {
        this->removeDockWidget(*i);
        delete *i;
    }
    dockWidgets.clear();
}

}
}
