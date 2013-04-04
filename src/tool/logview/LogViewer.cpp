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

void LogViewer::addProtoViewers(std::vector<unlog::GenericProviderModule*>
                                inputs)
{
    for(std::vector<unlog::GenericProviderModule*>::iterator i =
            inputs.begin(); i != inputs.end(); i++)
    {
        //if ((*i)->getType() == "messages.YUVImage") continue;

        QDockWidget* dockWidget =
            new QDockWidget(QString((*i)->getType().data()),
                                this);
        dockWidget->setMinimumWidth(300);
        dockWidget->setMaximumHeight(125);
        ProtoViewer* viewer = new ProtoViewer((*i)->getMessage(), dockWidget);

        QObject::connect(*i, SIGNAL(newMessage(const google::protobuf::Message*)),
                         viewer, SLOT(updateView(const google::protobuf::Message*)));

        dockWidget->setWidget(viewer);
        this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
    }
}

void LogViewer::addImageViewers(std::vector<image::YUVtoRGBModule*> inputs)
{
    for(std::vector<image::YUVtoRGBModule*>::iterator i =
            inputs.begin(); i != inputs.end(); i++)
    {
        image::ImageDisplayQModule* disp = new image::ImageDisplayQModule();
        disp->imageIn.wireTo(&(*i)->rgbOut);
        emit newDisplayModule(disp);
        imageTabs.addTab(disp, tr("An Image!"));
    }
}

}
}
