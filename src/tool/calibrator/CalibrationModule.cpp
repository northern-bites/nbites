#include "CalibrationModule.h"

namespace tool {
namespace calibrate {

CalibrationLineWrapperModule::CalibrationLineWrapperModule() :
    Module(),
    linesOut(base())
{
}

void CalibrationLineWrapperModule::run_()
{
    cameraIn.latch();
    locationIn.latch();

    portals::Message<messages::CalibrateLines> expected(0);

    std::vector<boost::shared_ptr<man::vision::VisualLine> > lines =
        vision.getExpectedLines(cameraIn.message(),
                                jointsIn.message(),
                                inertialIn.message(),
                                locationIn.message().x(),
                                locationIn.message().y(),
                                locationIn.message().h());

    for(std::vector<boost::shared_ptr<man::vision::VisualLine> >::const_iterator i =
            lines.begin(); i != lines.end(); i++)
    {
        messages::VisualLine *visLine = expected.get()->add_expected_line();
        visLine->mutable_visual_detection()->set_distance(i->get()->getDistance());
        visLine->mutable_visual_detection()->set_bearing(i->get()->getBearing());
        visLine->mutable_visual_detection()->set_distance_sd(i->get()->getDistanceSD());
        visLine->mutable_visual_detection()->set_bearing_sd(i->get()->getBearingSD());
        visLine->set_start_x(i->get()->getStartpoint().x);
        visLine->set_start_y(i->get()->getStartpoint().y);
        visLine->set_end_x(i->get()->getEndpoint().x);
        visLine->set_end_y(i->get()->getEndpoint().y);
        visLine->set_angle(i->get()->getAngle());
        visLine->set_avg_width(i->get()->getAvgWidth());
        visLine->set_length(i->get()->getLength());
        visLine->set_slope(i->get()->getSlope());
        visLine->set_y_int(i->get()->getYIntercept());
    }

    linesOut.setMessage(expected);
}

CalibrationModule::CalibrationModule(QWidget *parent) : QMainWindow(parent)
{
}

void CalibrationModule::run_()
{
}

}
}
