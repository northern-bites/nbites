
#include "TestImage.h"
#include <Qt>
#include <QPainter>

namespace qtool {
namespace image {
    using namespace man::memory::proto;
    using namespace man::memory;
    using namespace google::protobuf;

    TestImage::TestImage(MVision::const_ptr visionData) :
	visionData(visionData)

{ } 

void TestImage::updateBitmap() {
     const PVision::PVisualBall ballData = visionData->get()->visual_ball();
     int ball_x = 2*ballData.visual_detection().x();
     int ball_y = 2*ballData.visual_detection().y();
     int ball_radius = 2*ballData.radius();

    bitmap= QImage(640,480, QImage::Format_ARGB32);
    QPainter painter(&bitmap);
    painter.setBackgroundMode(Qt::TransparentMode);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(QColor::QColor(220,0,255,180), 3, Qt::SolidLine, Qt::FlatCap));
    painter.drawEllipse(ball_x,ball_y,2*ball_radius,2*ball_radius);
    //fill this

    // const RepeatedPtrField<PVision::PVisualCorner> cornerData = visionData->get()->visual_corner();
    
    //use iterator (for from 0 to size)
    //	PVision::PVisualCorner corner_location=cornerData.Get(cornerData.size()-1);
	
	//x at corners

    
    /*int corner_x=cornerData.visual_detection().x();
    int corner_y=cornerData.visual_detection().y();
    int corner_width=2*(cornerData.visual_detection().center_x()-corner_x);
    int corner_height= 2*(cornerData.visual_detection().center_y()-corner_y);
    painter.setPen(QPen(Qt::red, 3, Qt::SolidLine, Qt::FlatCap));
    painter.drawEllipse(corner_x, corner_y, corner_width, corner_height);*/
}

}
}
