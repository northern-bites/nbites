
#include "VisionDisplayModule.h"
#include <QFileDialog>


namespace tool {
namespace vision { 



VisionDisplayModule::VisionDisplayModule(QWidget *parent) :
	QWidget(parent),
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
	topThrDisplay.imageIn.wireTo(&topConverter.thrImage);
	botThrDisplay.imageIn.wireTo(&bottomConverter.thrImage);
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


	// GUI
    QHBoxLayout* mainLayout = new QHBoxLayout;
    QHBoxLayout* leftLayout = new QHBoxLayout;

	QToolBar* toolBar = new QToolBar(this);
    QPushButton* loadBtn = new QPushButton("Load", this);
    connect(loadBtn, SIGNAL(clicked()), this, SLOT(loadColorTable()));
	toolBar->addWidget(loadBtn);
	leftLayout->addWidget(toolBar); 

    imageTabs = new QTabWidget(this);
    leftLayout->addWidget(imageTabs);

    imageTabs->addTab(&topDisplay, "Top Image");
    imageTabs->addTab(&bottomDisplay, "Bottom Image");
	imageTabs->addTab(&topThrDisplay, "Top Thresh");
	imageTabs->addTab(&botThrDisplay, "Bottom Thresh");

	mainLayout->addLayout(leftLayout);
	
	setLayout(mainLayout);


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
