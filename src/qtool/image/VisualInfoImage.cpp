#include "VisualInfoImage.h"
#include <Qt>
#include <QPainter>

namespace qtool {
namespace image {

using namespace man::memory::proto;
using namespace man::memory;
using namespace google::protobuf;

VisualInfoImage::VisualInfoImage(MVision::const_ptr visionData) :
	        visionData(visionData)

{
    int image_width = 640;
    int image_height = 480;
    bitmap= QPixmap(image_width, image_height);
}

void VisualInfoImage::buildBitmap() {

    bitmap.fill(Qt::transparent);

    const PVision::PVisualBall ballData = visionData->get()->visual_ball();
    drawBall(ballData);

    const PVision::PVisualFieldObject bglpData = visionData->get()->bglp();
    drawGoalPost(bglpData);

    const PVision::PVisualFieldObject bgrpData = visionData->get()->bgrp();
    drawGoalPost(bgrpData);

    const PVision::PVisualFieldObject yglpData = visionData->get()->yglp();
    if(yglpData.visual_detection().distance() || yglpData.visual_detection().x() ||
            yglpData.visual_detection().y())
        drawGoalPost(yglpData);

    const PVision::PVisualFieldObject ygrpData = visionData->get()->ygrp();
    if(ygrpData.visual_detection().distance() || ygrpData.visual_detection().x() ||
            ygrpData.visual_detection().y())
        drawGoalPost(ygrpData);

    const PVision::PVisualRobot red1Data = visionData->get()->red1();
    if(red1Data.visual_detection().distance() || red1Data.visual_detection().x() ||
            red1Data.visual_detection().y())
        drawRedRobot(red1Data);

    const PVision::PVisualRobot red2Data = visionData->get()->red2();
    if(red2Data.visual_detection().distance() || red2Data.visual_detection().x() ||
            red2Data.visual_detection().y())
        drawRedRobot(red2Data);

    const PVision::PVisualRobot red3Data = visionData->get()->red3();
    if(red3Data.visual_detection().distance() || red3Data.visual_detection().x() ||
            red3Data.visual_detection().y())
        drawRedRobot(red3Data);

    const PVision::PVisualRobot navy1Data = visionData->get()->navy1();
    if(navy1Data.visual_detection().distance() || navy1Data.visual_detection().x() ||
            navy1Data.visual_detection().y())
        drawNavyRobot(navy1Data);

    const PVision::PVisualRobot navy2Data = visionData->get()->navy2();
    if(navy2Data.visual_detection().distance() || navy2Data.visual_detection().x() ||
            navy2Data.visual_detection().y())
        drawNavyRobot(navy2Data);

    const PVision::PVisualRobot navy3Data = visionData->get()->navy3();
    if(navy3Data.visual_detection().distance() || navy3Data.visual_detection().x() ||
            navy3Data.visual_detection().y())
        drawNavyRobot(navy3Data);

    const PVision::PVisualCross crossData = visionData->get()->visual_cross();
    if(crossData.visual_detection().distance() || crossData.visual_detection().x() ||
            crossData.visual_detection().y())
        drawCross(crossData);

    const RepeatedPtrField<PVision::PVisualCorner> cornersData = visionData->get()->visual_corner();
    for(int i=0; i<cornersData.size(); i++) {
        const PVision::PVisualCorner cornerData=cornersData.Get(i);
        drawCorner(cornerData);
    }

    const RepeatedPtrField<PVision::PVisualLine> linesData = visionData->get()->visual_line();
    for(int i=0; i<linesData.size(); i++) {
        const PVision::PVisualLine lineData=linesData.Get(i);
        drawLine(lineData);
    }
}

void VisualInfoImage::drawBall(const PVision::PVisualBall ballData) {
    QPainter painter(&bitmap);
    //    painter.setBackgroundMode(Qt::TransparentMode);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int ball_x = 2*ballData.visual_detection().x();
    int ball_y = 2*ballData.visual_detection().y();
    int ball_radius = 2*ballData.radius();

    painter.setPen(QPen(QColor(0,0,0,200), 3, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(QBrush(QColor(255,0,0,80),Qt::SolidPattern));
    painter.drawEllipse(ball_x,ball_y,2*ball_radius,2*ball_radius);
}

void VisualInfoImage::drawCorner(const PVision::PVisualCorner cornerData) {
    QPainter painter(&bitmap);
    //    painter.setBackgroundMode(Qt::TransparentMode);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int corner_x=cornerData.visual_detection().x();
    int corner_y=cornerData.visual_detection().y();
    int corner_width=2*(cornerData.visual_detection().center_x()-corner_x);
    int corner_height= 2*(cornerData.visual_detection().center_y()-corner_y);

    painter.setPen(QPen(Qt::red, 3, Qt::SolidLine, Qt::FlatCap));
    painter.drawLine(2*corner_x-10, 2*corner_y-10, 2*corner_x+10, 2*corner_y+10);
    painter.drawLine(2*corner_x+10, 2*corner_y-10, 2*corner_x-10, 2*corner_y+10);

}

void VisualInfoImage::drawGoalPost(const PVision::PVisualFieldObject postData) {
    QPainter painter(&bitmap);
    //    painter.setBackgroundMode(Qt::TransparentMode);
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
    
    painter.setPen(QPen(QColor(0,0,0,200), 3, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(QBrush(QColor(255,255,0,80),Qt::SolidPattern));
    painter.drawConvexPolygon(points, 4);
}

  void VisualInfoImage::drawNavyRobot(const PVision::PVisualRobot robotData) {
    QPainter painter(&bitmap);
//    painter.setBackgroundMode(Qt::TransparentMode);
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
    
    painter.setPen(QPen(QColor(0,0,0,200), 3, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(QBrush(QColor(0,0,200,80),Qt::SolidPattern));
    painter.drawConvexPolygon(points, 4);
  }
 void VisualInfoImage::drawRedRobot(const PVision::PVisualRobot robotData) {
    QPainter painter(&bitmap);
//    painter.setBackgroundMode(Qt::TransparentMode);
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
    
    painter.setPen(QPen(QColor(0,0,0,200), 3, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(QBrush(QColor(200,0,0,80),Qt::SolidPattern));
    painter.drawConvexPolygon(points, 4);
  }

  void VisualInfoImage::drawLine(const PVision::PVisualLine lineData) {
    QPainter painter(&bitmap);
//    painter.setBackgroundMode(Qt::TransparentMode);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int start_x = 2*lineData.start_x();
    int start_y = 2*lineData.start_y();
    int end_x = 2*lineData.end_x();
    int end_y = 2*lineData.end_y();
    
    painter.setPen(QPen(Qt::blue, 6, Qt::SolidLine, Qt::FlatCap));
    painter.drawLine(start_x, start_y, end_x, end_y);
  }

  void VisualInfoImage::drawCross(const PVision::PVisualCross crossData) {
    QPainter painter(&bitmap);
//    painter.setBackgroundMode(Qt::TransparentMode);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int left_top_x = 2*crossData.left_top_x();
    int left_top_y = 2*crossData.left_top_y();
    int left_bottom_x = 2*crossData.left_bottom_x();
    int left_bottom_y = 2*crossData.left_bottom_y();
    int right_top_x = 2*crossData.right_top_x();
    int right_top_y = 2*crossData.right_top_y();
    int right_bottom_x = 2*crossData.right_bottom_x();
    int right_bottom_y = 2*crossData.right_bottom_y();

    painter.setPen(QPen(Qt::cyan, 3, Qt::SolidLine, Qt::FlatCap));
    painter.drawLine(left_top_x, left_top_y, right_bottom_x, right_bottom_y);
    painter.drawLine(right_top_x, right_top_y, left_bottom_x, left_bottom_y);
    

  }
}
}
