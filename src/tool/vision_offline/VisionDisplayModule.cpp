#include "VisionDisplayModule.h"
#include <QFileDialog>


namespace tool {
namespace vision {



VisionDisplayModule::VisionDisplayModule(QWidget *parent) :
    QMainWindow(parent),
    loadCalButton(new QPushButton(tr("Load Calibration Values"))),
    robotNames(new QComboBox()),
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
    joints(base()),
    inertials(base()),
    visMod()
{

    subdiagram.addModule(topDisplay);
    subdiagram.addModule(bottomDisplay);
    subdiagram.addModule(topThrDisplay);
    subdiagram.addModule(botThrDisplay);
    subdiagram.addModule(visMod);

    topDisplay.imageIn.wireTo(&topImage, true);
    bottomDisplay.imageIn.wireTo(&bottomImage, true);
    topThrDisplay.imageIn.wireTo(&visMod.topOutPic);
    botThrDisplay.imageIn.wireTo(&visMod.botOutPic);

    visMod.topThrImage.wireTo(&tTImage, true);
    visMod.topYImage.wireTo(&tYImage, true);
    visMod.topUImage.wireTo(&tUImage, true);
    visMod.topVImage.wireTo(&tVImage, true);

    visMod.botThrImage.wireTo(&bTImage, true);
    visMod.botYImage.wireTo(&bYImage, true);
    visMod.botUImage.wireTo(&bUImage, true);
    visMod.botVImage.wireTo(&bVImage, true);

    visMod.joint_angles.wireTo(&joints, true);
    visMod.inertial_state.wireTo(&inertials, true);


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

    robotNames->addItem("");
    robotNames->addItem("river");
    robotNames->addItem("mal");
    robotNames->addItem("wash");
    robotNames->addItem("zoe");
    robotNames->addItem("jayne");
    robotNames->addItem("inara");
    robotNames->addItem("vera");
    robotNames->addItem("simon");
    robotNames->addItem("kaylee");

    QToolBar* toolBar = new QToolBar(this);
    toolBar->addWidget(loadCalButton);
    toolBar->addWidget(robotNames);
    connect(loadCalButton, SIGNAL(clicked(bool)),
            this, SLOT(loadRobotParameters()));
    this->addToolBar(toolBar);

}

void VisionDisplayModule::loadRobotParameters()
{
    std::string name = robotNames->currentText().toStdString();
    CameraCalibrate::UpdateByName(name);
    subdiagram.run();
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

    joints_in.latch();
    inerts_in.latch();

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

    joints.setMessage(portals::Message<messages::JointAngles>(
                          &joints_in.message()));
    inertials.setMessage(portals::Message<messages::InertialState>(
                             &inerts_in.message()));

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
	const messages::VisionRobot *visRobots = visMod.vision_robot.getMessage(true).get();

	if (which == Camera::TOP) {
		for (int i = 0; i < visField->visual_line_size(); i++) {

			painter.drawLine(visField->visual_line(i).start_x(),
							 visField->visual_line(i).start_y(),
							 visField->visual_line(i).end_x(),
							 visField->visual_line(i).end_y());
		}


		painter.setPen(Qt::magenta);
		for (int i = 0; i < visField->visual_corner_size(); i++) {
			switch(visField->visual_corner(i).corner_type()) {
			case messages::VisualCorner::INNER_L: painter.setPen(Qt::green);
				break;
			case messages::VisualCorner::OUTER_L: painter.setPen(Qt::blue);
				break;
			case messages::VisualCorner::T: painter.setPen(Qt::black);
				break;
			case messages::VisualCorner::CIRCLE: painter.setPen(Qt::magenta);
				break;
			}
			painter.drawLine(visField->visual_corner(i).x() - 5,
							 visField->visual_corner(i).y() - 5,
							 visField->visual_corner(i).x() + 5,
							 visField->visual_corner(i).y() + 5);
			painter.drawLine(visField->visual_corner(i).x() + 5,
							 visField->visual_corner(i).y() - 5,
							 visField->visual_corner(i).x() - 5,
							 visField->visual_corner(i).y() + 5);
		}

		if (visBall->intopcam()) {
			int ball_x = visBall->x();
			int ball_y = visBall->y();
			int ball_radius = visBall->radius();

			painter.setPen(QPen(QColor(0,0,255,200), 1, Qt::SolidLine, Qt::FlatCap));
			painter.setBrush(QBrush(QColor(255,0,0,80),Qt::SolidPattern));
			painter.drawEllipse(ball_x,ball_y,2*ball_radius,2*ball_radius);
		}

		const messages::Robot red1 = visRobots->red1();
		const messages::Robot navy1 = visRobots->navy1();

		painter.setPen(QColor(0,0,0));
		painter.setBrush(QBrush(QColor(0, 0, 255, 0), Qt::SolidPattern));
		painter.setPen(QPen(QColor(0,255,0,200), 1, Qt::SolidLine, Qt::FlatCap));
		painter.setBrush(QBrush(QColor(0,255,0,80), Qt::SolidPattern));
		if (red1.distance() > 0) {
            // painter.drawLine(red1.x() + 10, red1.y() + 10, red1.x(), red1.y());
            // painter.drawLine(red1.x(), red1.y() + 10, red1.x() + 10, red1.y());
			QPoint r_points[4] = {
				QPoint(red1.x(), red1.y()),
				QPoint(red1.x()+red1.width(), red1.y()),
				QPoint(red1.x()+red1.width(), red1.y()+red1.height()),
				QPoint(red1.x(), red1.y()+red1.height())
				};
				painter.drawConvexPolygon(r_points, 4);
		}
		painter.setBrush(QBrush(QColor(255, 0, 0, 0), Qt::SolidPattern));
		painter.setBrush(QBrush(QColor(255,0,0,80), Qt::SolidPattern));
		if (navy1.on()) {
            painter.drawLine(navy1.x() + 10, navy1.y() + 10, navy1.x(), navy1.y());
            painter.drawLine(navy1.x(), navy1.y() + 10, navy1.x() + 10, navy1.y());
			QPoint r_points[4] = {
				QPoint(navy1.x(), navy1.y()),
				QPoint(navy1.x()+navy1.width(), navy1.y()),
				QPoint(navy1.x()+navy1.width(), navy1.y()+navy1.height()),
				QPoint(navy1.x(), navy1.y()+navy1.height())
			};
			painter.drawConvexPolygon(r_points, 4);
		}

		const messages::VisualGoalPost yglp = visField->goal_post_l();
		const messages::VisualGoalPost ygrp = visField->goal_post_r();
		// Now we are to draw the goal posts
		painter.setPen(QColor(0,0,0,200));
		painter.setBrush(QBrush(QColor(255,255,0,80), Qt::SolidPattern));
		if (ygrp.visual_detection().certainty() == 2)
			painter.setPen(QColor(0,0,255,200));
		if (ygrp.visual_detection().on() && ygrp.visual_detection().intopcam()) {
			QPoint r_points[4] = {
				QPoint(ygrp.left_top().x(), ygrp.left_top().y()),
				QPoint(ygrp.right_top().x(), ygrp.right_top().y()),
				QPoint(ygrp.right_bot().x(), ygrp.right_bot().y()),
				QPoint(ygrp.left_bot().x(), ygrp.left_bot().y())
			};
			painter.drawConvexPolygon(r_points, 4);
		}
		if (yglp.visual_detection().on() && yglp.visual_detection().intopcam()) {
			painter.setPen(QColor(255,0,0,200));
			QPoint l_points[4] = {
				QPoint(yglp.left_top().x(), yglp.left_top().y()),
				QPoint(yglp.right_top().x(), yglp.right_top().y()),
				QPoint(yglp.right_bot().x(), yglp.right_bot().y()),
				QPoint(yglp.left_bot().x(), yglp.left_bot().y())
			};
			painter.drawConvexPolygon(l_points, 4);
		}

        painter.setPen(Qt::red);
        if (visField->visual_cross().distance() > 0) {
            painter.drawLine(visField->visual_cross().x() + 10,
                             visField->visual_cross().y() + 10,
                             visField->visual_cross().x(),
                             visField->visual_cross().y());
            painter.drawLine(visField->visual_cross().x(),
                             visField->visual_cross().y() + 10,
                             visField->visual_cross().x() + 10,
                             visField->visual_cross().y());
        }


	}
	else { // this is to draw in the bottom camera

		if (!visBall->intopcam()) {
			int ball_x = visBall->x();
			int ball_y = visBall->y();
			int ball_radius = visBall->radius();

			painter.setPen(QPen(QColor(0,0,255,200), 1, Qt::SolidLine, Qt::FlatCap));
			painter.setBrush(QBrush(QColor(255,0,0,80),Qt::SolidPattern));
			painter.drawEllipse(ball_x,ball_y,2*ball_radius,2*ball_radius);
		}
		const messages::VisualGoalPost yglp = visField->goal_post_l();
		const messages::VisualGoalPost ygrp = visField->goal_post_r();
		// Now we are to draw the goal posts
		painter.setPen(QColor(0,0,0,200));
		painter.setBrush(QBrush(QColor(255,255,0,80), Qt::SolidPattern));
		if (ygrp.visual_detection().certainty() == 2)
			painter.setPen(QColor(0,0,255,200));
		if (ygrp.visual_detection().on() && !ygrp.visual_detection().intopcam()) {
			QPoint r_points[4] = {
				QPoint(ygrp.left_top().x(), ygrp.left_top().y()),
				QPoint(ygrp.right_top().x(), ygrp.right_top().y()),
				QPoint(ygrp.right_bot().x(), ygrp.right_bot().y()),
				QPoint(ygrp.left_bot().x(), ygrp.left_bot().y())
			};
			painter.drawConvexPolygon(r_points, 4);
		}
		if (yglp.visual_detection().on() && !yglp.visual_detection().intopcam()) {
			painter.setPen(QColor(255,0,0,200));
			QPoint l_points[4] = {
				QPoint(yglp.left_top().x(), yglp.left_top().y()),
				QPoint(yglp.right_top().x(), yglp.right_top().y()),
				QPoint(yglp.right_bot().x(), yglp.right_bot().y()),
				QPoint(yglp.left_bot().x(), yglp.left_bot().y())
			};
			painter.drawConvexPolygon(l_points, 4);
		}
	}

	return lineImage;

}


}
}
