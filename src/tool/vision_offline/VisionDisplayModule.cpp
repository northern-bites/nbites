#include "VisionDisplayModule.h"
#include <QFileDialog>


namespace tool {
namespace vision {



VisionDisplayModule::VisionDisplayModule(QWidget *parent) :
    QMainWindow(parent),
    currentCamera(Camera::TOP),
    topDisplay(this),
    bottomDisplay(this),
    topThrDisplay(this),
    botThrDisplay(this),
    bottomImage(base()),
    topImage(base()),
    tTImage(base()),
    tYImage(base()),
    tUImage(base()),
    tVImage(base()),
    bTImage(base()),
    bYImage(base()),
    bUImage(base()),
    bVImage(base()),
    visMod()
{

subdiagram.addModule(topDisplay);
    subdiagram.addModule(bottomDisplay);
    subdiagram.addModule(topThrDisplay);
    subdiagram.addModule(botThrDisplay);
    subdiagram.addModule(visMod);

    topDisplay.imageIn.wireTo(&topImage, true);
    bottomDisplay.imageIn.wireTo(&bottomImage, true);
    topThrDisplay.imageIn.wireTo(&visMod.topOutPic, true);
    botThrDisplay.imageIn.wireTo(&visMod.botOutPic, true);
    // Dummy Sensors messages for VisMod
    portals::Message<messages::JointAngles> joints(0);
    portals::Message<messages::InertialState> inertials(0);

    visMod.topThrImage.wireTo(&tTImage, true);
    visMod.topYImage.wireTo(&tYImage, true);
    visMod.topUImage.wireTo(&tUImage, true);
    visMod.topVImage.wireTo(&tVImage, true);

    visMod.botThrImage.wireTo(&bTImage, true);
    visMod.botYImage.wireTo(&bYImage, true);
    visMod.botUImage.wireTo(&bUImage, true);
    visMod.botVImage.wireTo(&bVImage, true);

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

    tTImage_in.latch();
    tYImage_in.latch();
    tUImage_in.latch();
    tVImage_in.latch();
    bTImage_in.latch();
    bYImage_in.latch();
    bUImage_in.latch();
    bVImage_in.latch();



    bottomImage.setMessage(portals::Message<messages::YUVImage>(
                               &bottomImageIn.message()));
    topImage.setMessage(portals::Message<messages::YUVImage>(
                            &topImageIn.message()));

    tTImage.setMessage(portals::Message<messages::ThresholdImage>(
                           &tTImage_in.message()));
    tYImage.setMessage(portals::Message<messages::PackedImage16>(
                           &tYImage_in.message()));
    tUImage.setMessage(portals::Message<messages::PackedImage16>(
                           &tUImage_in.message()));
    tVImage.setMessage(portals::Message<messages::PackedImage16>(
                           &tVImage_in.message()));
    bTImage.setMessage(portals::Message<messages::ThresholdImage>(
                           &bTImage_in.message()));
    bYImage.setMessage(portals::Message<messages::PackedImage16>(
                           &bYImage_in.message()));
    bUImage.setMessage(portals::Message<messages::PackedImage16>(
                           &bUImage_in.message()));
    bVImage.setMessage(portals::Message<messages::PackedImage16>(
                           &bVImage_in.message()));

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
    const messages::VisionBall *visBall = visMod.vision_ball.getMessage(true).get();

    if (which == Camera::TOP) {
//        std::cout << "there are " << visField->visual_line_size() << " lines in the image\n";
        for (int i = 0; i < visField->visual_line_size(); i++) {

            painter.drawLine(visField->visual_line(i).start_x(),
                             visField->visual_line(i).start_y(),
                             visField->visual_line(i).end_x(),
                             visField->visual_line(i).end_y());
        }


        if (visBall->intopcam()) {
            int ball_x = visBall->x();
            int ball_y = visBall->y();
            int ball_radius = visBall->radius();

//            std::cout << "ball_x = " << ball_x << std::endl;

            painter.setPen(QPen(QColor(0,0,255,200), 3, Qt::SolidLine, Qt::FlatCap));
            painter.setBrush(QBrush(QColor(255,0,0,80),Qt::SolidPattern));
            painter.drawEllipse(ball_x,ball_y,2*ball_radius,2*ball_radius);
        }

    }

    return lineImage;

}


}
}
