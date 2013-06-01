
#include "VisionDisplayModule.h"
#include <QFileDialog>


namespace tool {
namespace vision { 



VisionDisplayModule::VisionDisplayModule(QWidget *parent) :
	QMainWindow(parent),
	currentCamera(Camera::TOP),
	topConverter(Camera::TOP),
	bottomConverter(Camera::BOTTOM),
	topDisplay(this),
	bottomDisplay(this),
	topThrDisplay(this),
	botThrDisplay(this),
	bottomImage(base()),
	topImage(base()),
	visMod()
{

	subdiagram.addModule(topConverter);
	subdiagram.addModule(bottomConverter);
	subdiagram.addModule(topDisplay);
	subdiagram.addModule(bottomDisplay);
	subdiagram.addModule(topThrDisplay);
	subdiagram.addModule(botThrDisplay);
	subdiagram.addModule(visMod);

	topConverter.imageIn.wireTo(&topImage, true);
	bottomConverter.imageIn.wireTo(&bottomImage, true);
	topDisplay.imageIn.wireTo(&topImage, true);
	bottomDisplay.imageIn.wireTo(&bottomImage, true);
	topThrDisplay.imageIn.wireTo(&visMod.topOutPic, true);
	botThrDisplay.imageIn.wireTo(&visMod.botOutPic, true);
	// Dummy Sensors messages for VisMod
	portals::Message<messages::JointAngles> joints(0);
	portals::Message<messages::InertialState> inertials(0);
	
	visMod.topThrImage.wireTo(&topConverter.thrImage);
    visMod.topYImage.wireTo(&topConverter.yImage);
    visMod.topUImage.wireTo(&topConverter.uImage);
    visMod.topVImage.wireTo(&topConverter.vImage);

    visMod.botThrImage.wireTo(&bottomConverter.thrImage);
    visMod.botYImage.wireTo(&bottomConverter.yImage);
    visMod.botUImage.wireTo(&bottomConverter.uImage);
    visMod.botVImage.wireTo(&bottomConverter.vImage);

    visMod.joint_angles.setMessage(joints);
	visMod.inertial_state.setMessage(inertials);


	field_viewer = new logview::TypedProtoViewer<messages::VisionField>();
	field_viewer->input.wireTo(&visMod.vision_field);
	subdiagram.addModule(*field_viewer);
	robot_viewer = new logview::TypedProtoViewer<messages::VisionRobot>();
	robot_viewer->input.wireTo(&visMod.vision_robot);
	subdiagram.addModule(*robot_viewer);
	ball_viewer = new logview::TypedProtoViewer<messages::VisionBall>();
	ball_viewer->input.wireTo(&visMod.vision_ball);
	subdiagram.addModule(*ball_viewer);
	obstacle_viewer = new logview::TypedProtoViewer<messages::VisionObstacle>();
	obstacle_viewer->input.wireTo(&visMod.vision_obstacle);
	subdiagram.addModule(*obstacle_viewer);
	
	QDockWidget* dockWidget = new QDockWidget("Vision Field", this);
	dockWidget->setMinimumWidth(300);
	dockWidget->setWidget(field_viewer);
	this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
	
	dockWidget = new QDockWidget("Vision Robot", this);
	dockWidget->setMinimumWidth(300);
	dockWidget->setWidget(robot_viewer);
	this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);

	dockWidget = new QDockWidget("Vision Ball", this);
	dockWidget->setMinimumWidth(300);
	dockWidget->setWidget(ball_viewer);
	this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);

	dockWidget = new QDockWidget("Vision Obstacle", this);
	dockWidget->setMinimumWidth(300);
	dockWidget->setWidget(obstacle_viewer);
	this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);

	// GUI
    QHBoxLayout* mainLayout = new QHBoxLayout;
	QWidget* mainWidget = new QWidget;

	QToolBar* toolBar = new QToolBar(this);
    QPushButton* loadBtn = new QPushButton("Load Table", this);
    connect(loadBtn, SIGNAL(clicked()), this, SLOT(loadColorTable()));
	toolBar->addWidget(loadBtn);
	this->addToolBar(toolBar); 

    imageTabs = new QTabWidget(this);
    mainLayout->addWidget(imageTabs);

    imageTabs->addTab(&topDisplay, "Top Image");
    imageTabs->addTab(&bottomDisplay, "Bottom Image");
	imageTabs->addTab(&topThrDisplay, "Top Thresh");
	imageTabs->addTab(&botThrDisplay, "Bottom Thresh");
	
	mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	mainWidget->setLayout(mainLayout);
	this->setCentralWidget(mainWidget);

    //corner ownership
    this->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

}

void VisionDisplayModule::run_()
{
    bottomImageIn.latch();
    topImageIn.latch();

    bottomImage.setMessage(portals::Message<messages::YUVImage>(
                               &bottomImageIn.message()));
    topImage.setMessage(portals::Message<messages::YUVImage>(
                            &topImageIn.message()));
	
	topConverter.initTable(colorTable.getTable());
    bottomConverter.initTable(colorTable.getTable());

    subdiagram.run();
	
	topDisplay.setOverlay(makeOverlay(Camera::TOP));
	bottomDisplay.setOverlay(makeOverlay(Camera::BOTTOM));
	
}

QImage VisionDisplayModule::makeOverlay(Camera::Type which)
{
	QImage lineImage(320, 240, QImage::Format_ARGB32);
    lineImage.fill(qRgba(0, 0, 0, 0));
    QPainter painter(&lineImage);
    painter.setPen(QColor(246, 15, 15));

	const messages::VisionField *visField = visMod.vision_field.getMessage(true).get();

	if (which == Camera::TOP) {
		for (int i = 0; i < visField->visual_line_size(); i++) {
			
			painter.drawLine(visField->visual_line(i).start_x(),
							 visField->visual_line(i).start_y(),
							 visField->visual_line(i).end_x(),
							 visField->visual_line(i).end_y());
		}
	}
	
	return lineImage;

}

void VisionDisplayModule::loadColorTable()
{
    QString base_directory = QString(NBITES_DIR) + "/data/tables";
    QString filename = QFileDialog::getOpenFileName(this,
                    tr("Load Color Table from File"),
                    base_directory,
                    tr("Color Table files (*.mtb)"));
    colorTable.read(filename.toStdString());
}

}
}
