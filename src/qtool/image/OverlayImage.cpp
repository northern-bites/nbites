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
     const PVision::PVisualBall ballData = visionData->get()->visual_ball();
     int ball_x = 2*ballData.visual_detection().x();
     int ball_y = 2*ballData.visual_detection().y();
     int ball_radius = 2*ballData.radius();
     int image_width = 640;
     int image_height = 480;

     bitmap= QImage(image_width,image_height, QImage::Format_ARGB32);
     QPainter painter(&bitmap);
     painter.setBackgroundMode(Qt::TransparentMode);
     painter.setRenderHint(QPainter::Antialiasing, true);
     painter.setPen(QPen(QColor::QColor(0,0,0,200), 3, Qt::SolidLine, Qt::FlatCap));
     painter.setBrush(QBrush(QColor::QColor(220,0,255,80),Qt::SolidPattern));
     painter.drawEllipse(ball_x,ball_y,2*ball_radius,2*ball_radius);

     const RepeatedPtrField<PVision::PVisualCorner> cornerData = visionData->get()->visual_corner();
     for(int i=0; i<cornerData.size(); i++) {
       const PVision::PVisualCorner cornerDatum=cornerData.Get(i);
       int corner_x=cornerDatum.visual_detection().x();
       int corner_y=cornerDatum.visual_detection().y();
       int corner_width=2*(cornerDatum.visual_detection().center_x()-corner_x);
       int corner_height= 2*(cornerDatum.visual_detection().center_y()-corner_y);
       painter.setPen(QPen(Qt::red, 3, Qt::SolidLine, Qt::FlatCap));
       painter.setBrush(QBrush(QColor::QColor(255,255,255,0),Qt::SolidPattern));
       painter.drawLine(2*corner_x-10, 2*corner_y-10, 2*corner_x+10, 2*corner_y+10);
       painter.drawLine(2*corner_x+10, 2*corner_y-10, 2*corner_x-10, 2*corner_y+10);
     }     
}

}
}
