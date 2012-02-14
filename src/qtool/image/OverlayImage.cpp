#include "OverlayImage.h"
#include <Qt>
#include <QPainter>

namespace qtool {
namespace image {
    using namespace man::memory::proto;
    using namespace man::memory;
    using namespace google::protobuf;

    OverlayImage::OverlayImage(MVision::const_ptr visionData) :
	visionData(visionData)

{ } 

void OverlayImage::updateBitmap() {
     int image_width = 640;
     int image_height = 480;
     bitmap= QImage(image_width,image_height, QImage::Format_ARGB32);
     
     const PVision::PVisualBall ballData = visionData->get()->visual_ball();
     drawBall(ballData);

     const PVision::PVisualFieldObject bglpData = visionData->get()->bglp();
     drawGoalPost(bglpData);
     
     const PVision::PVisualFieldObject bgrpData = visionData->get()->bgrp();
     drawGoalPost(bgrpData);
     
     const PVision::PVisualFieldObject yglpData = visionData->get()->yglp();
     drawGoalPost(yglpData);

     const PVision::PVisualFieldObject ygrpData = visionData->get()->ygrp();
     drawGoalPost(ygrpData);
     
     const PVision::PVisualRobot red1Data = visionData->get()->red1();
     drawRobot(red1Data);
     
     const PVision::PVisualRobot red2Data = visionData->get()->red2();
     drawRobot(red2Data);

     const PVision::PVisualRobot red3Data = visionData->get()->red3();
     drawRobot(red3Data);

     const PVision::PVisualRobot navy1Data = visionData->get()->navy1();
     drawRobot(navy1Data);
     
     const PVision::PVisualRobot navy2Data = visionData->get()->navy2();
     drawRobot(navy2Data);
     
     const PVision::PVisualRobot navy3Data = visionData->get()->navy3();
     drawRobot(navy3Data);
     
     const RepeatedPtrField<PVision::PVisualCorner> cornerData = visionData->get()->visual_corner();
     for(int i=0; i<cornerData.size(); i++) {
       const PVision::PVisualCorner cornerDatum=cornerData.Get(i);
       drawCorner(cornerDatum);
     }
}

void OverlayImage::drawBall(const PVision::PVisualBall ballData) {
    QPainter painter(&bitmap);
    painter.setBackgroundMode(Qt::TransparentMode);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int ball_x = 2*ballData.visual_detection().x();
    int ball_y = 2*ballData.visual_detection().y();
    int ball_radius = 2*ballData.radius();
     
    painter.setPen(QPen(QColor::QColor(0,0,0,200), 3, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(QBrush(QColor::QColor(255,0,0,80),Qt::SolidPattern));
    painter.drawEllipse(ball_x,ball_y,2*ball_radius,2*ball_radius);
  }

void OverlayImage::drawCorner(const PVision::PVisualCorner cornerDatum) {
    QPainter painter(&bitmap);
    painter.setBackgroundMode(Qt::TransparentMode);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int corner_x=cornerDatum.visual_detection().x();
    int corner_y=cornerDatum.visual_detection().y();
    int corner_width=2*(cornerDatum.visual_detection().center_x()-corner_x);
    int corner_height= 2*(cornerDatum.visual_detection().center_y()-corner_y);

    painter.setPen(QPen(Qt::red, 3, Qt::SolidLine, Qt::FlatCap));
    painter.drawLine(2*corner_x-10, 2*corner_y-10, 2*corner_x+10, 2*corner_y+10);
    painter.drawLine(2*corner_x+10, 2*corner_y-10, 2*corner_x-10, 2*corner_y+10);
    
  }
  
void OverlayImage::drawGoalPost(const PVision::PVisualFieldObject postData) {
    QPainter painter(&bitmap);
    painter.setBackgroundMode(Qt::TransparentMode);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int left_top_x = 2*postData.left_top_x();
    int left_top_y = 2*postData.left_top_y();
    int left_bottom_x = 2*postData.left_bottom_x();
    int left_bottom_y = 2*postData.left_bottom_y();
    int right_top_x = 2*postData.right_top_x();
    int right_top_y = 2*postData.right_top_y();
    int right_bottom_x = 2*postData.right_bottom_x();
    int right_bottom_y = 2*postData.right_bottom_y();

    QPoint points [4]= {
      QPoint (left_top_x, left_top_y),
      QPoint (left_bottom_x, left_bottom_y),
      QPoint (right_bottom_x, right_bottom_y),
      QPoint (right_top_x, right_top_y)
    };
    
    painter.setPen(QPen(QColor::QColor(0,0,0,200), 3, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(QBrush(QColor::QColor(255,255,0,80),Qt::SolidPattern));
    painter.drawConvexPolygon(points, 4);
}

  //draw robots-MAKE SEPERATE METHODS FOR DIFF COLORS?
  void OverlayImage::drawRobot(const PVision::PVisualRobot robotData) {
    QPainter painter(&bitmap);
    painter.setBackgroundMode(Qt::TransparentMode);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int left_top_x = 2*robotData.left_top_x();
    int left_top_y = 2*robotData.left_top_y();
    int left_bottom_x = 2*robotData.left_bottom_x();
    int left_bottom_y = 2*robotData.left_bottom_y();
    int right_top_x = 2*robotData.right_top_x();
    int right_top_y = 2*robotData.right_top_y();
    int right_bottom_x = 2*robotData.right_bottom_x();
    int right_bottom_y = 2*robotData.right_bottom_y();

    QPoint points [4]= {
      QPoint (left_top_x, left_top_y),
      QPoint (left_bottom_x, left_bottom_y),
      QPoint (right_bottom_x, right_bottom_y),
      QPoint (right_top_x, right_top_y)
    };
    
    painter.setPen(QPen(QColor::QColor(0,0,0,200), 3, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(QBrush(QColor::QColor(0,0,200,80),Qt::SolidPattern));
    painter.drawConvexPolygon(points, 4);
  }
}
}
