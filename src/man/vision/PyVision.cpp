/*
 * Wrapper for all vision information needed in Python. See header for list
 * of wrapped classes. In Python, import vision to use these. Generally
 * accessed through a FieldObject (see CombinationObjects) or through
 * brain.vision (an instance of the main vision class).
 */

#include <boost/shared_ptr.hpp>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using namespace boost::python;
using namespace std;

#include "PyVision.h"

boost::shared_ptr<Vision> vision_pointer;

BOOST_PYTHON_MODULE(vision)
{
    class_<VisualBall>("Ball", no_init)
        // Note that getters are in VisualDetection, parent to VisualBall
        .def_readonly("dist", &VisualBall::getDistance)
        .def_readonly("bearing", &VisualBall::getBearingDeg)
        .def_readonly("angleX", &VisualBall::getAngleXDeg)
        .def_readonly("angleY", &VisualBall::getAngleYDeg)
        .def_readonly("elevation", &VisualBall::getElevationDeg)
        // From VisualBall itself
        .def_readonly("confidence", &VisualBall::getConfidence)
        .def_readonly("radius", &VisualBall::getRadius)
        .def_readonly("heat", &VisualBall::getHeat)
        .def_readonly("on", &VisualBall::isOn)
        .def_readonly("framesOn", &VisualBall::getFramesOn)
        .def_readonly("framesOff", &VisualBall::getFramesOff)
        ;

    class_<VisualCross, boost::shared_ptr<VisualCross> >("Cross", no_init)
        .def_readonly("dist", &VisualCross::getDistance)
        .def_readonly("bearing", &VisualCross::getBearingDeg)
        .def_readonly("on", &VisualCross::isOn)
        .def_readonly("framesOn", &VisualCross::getFramesOn)
        .def_readonly("framesOff", &VisualCross::getFramesOff)
        ;

    class_<VisualFieldObject, boost::shared_ptr<VisualFieldObject> >
        ("FieldObject", no_init)
        // From VisualDetection
        .def_readonly("dist", &VisualFieldObject::getDistance)
        .def_readonly("bearing", &VisualFieldObject::getBearingDeg)
        .def_readonly("angleX", &VisualFieldObject::getAngleXDeg)
        .def_readonly("angleY", &VisualFieldObject::getAngleYDeg)
        // From VisualLandmark
        .def_readonly("certainty", &VisualFieldObject::getIDCertainty)
        .def_readonly("distCertainty", &VisualFieldObject::getDistanceCertainty)
        // From VisualFieldObject
        .def_readonly("on", &VisualFieldObject::isOn)
        .def_readonly("framesOn", &VisualFieldObject::getFramesOn)
        .def_readonly("framesOff", &VisualFieldObject::getFramesOff)
        ;

    // From VisualLandmark.h, ID certainty possibilities
    enum_<certainty>("certainty")
        .value("NOT_SURE", NOT_SURE)
        .value("MILDLY_SURE", MILDLY_SURE)
        .value("_SURE", _SURE)
        ;

    // From VisualLandmark.h, distance certainty possibilities
    enum_<distanceCertainty>("distanceCertainty")
        .value("BOTH_UNSURE", BOTH_UNSURE)
        .value("WIDTH_UNSURE", WIDTH_UNSURE)
        .value("HEIGHT_UNSURE", HEIGHT_UNSURE)
        .value("BOTH_SURE", BOTH_SURE)
        ;

    class_<VisualFieldEdge>("VisualFieldEdge", no_init)
        .def_readonly("maxDist", &VisualFieldEdge::getMaxDistance)
        .def_readonly("leftDist", &VisualFieldEdge::getDistanceLeft)
        .def_readonly("rightDist", &VisualFieldEdge::getDistanceRight)
        .def_readonly("centerDist", &VisualFieldEdge::getDistanceCenter)
        .def_readonly("shape", &VisualFieldEdge::getShape)
        ;

    // From VisualFieldEdge.h, describes shape of field edge
    enum_<Basic_Shape>("basicShape")
        .value("RISING_RIGHT", RISING_RIGHT)
        .value("RISING_LEFT", RISING_LEFT)
        .value("FLAT", FLAT)
        .value("WEDGE", WEDGE)
        ;

    class_<VisualRobot>("VisualRobot", no_init)
        // From VisualDetection
        .def_readonly("dist", &VisualRobot::getDistance)
        .def_readonly("bearing", &VisualRobot::getBearingDeg)
        .def_readonly("angleX", &VisualRobot::getAngleXDeg)
        .def_readonly("angleY", &VisualRobot::getAngleYDeg)
        .def_readonly("cornerX", &VisualRobot::getX)
        .def_readonly("cornerY", &VisualRobot::getY)
        .def_readonly("elevation", &VisualRobot::getElevationDeg)
        .def_readonly("on", &VisualRobot::isOn)
        ;

    // Currently unused, but fully avaliable to python if uncommented
    class_<VisualCrossbar>("Crossbar", no_init)
        // From VisualDetection
        .def_readonly("dist", &VisualCrossbar::getDistance)
        .def_readonly("bearing", &VisualCrossbar::getBearingDeg)
        .def_readonly("angleX", &VisualCrossbar::getAngleXDeg)
        .def_readonly("angleY", &VisualCrossbar::getAngleYDeg)
        .def_readonly("elevation", &VisualCrossbar::getElevationDeg)
        // From VisualCrossbar
        .def_readonly("shoot", &VisualCrossbar::shotAvailable)
        .def_readonly("leftOpening", &VisualCrossbar::getLeftOpening)
        .def_readonly("rightOpening", &VisualCrossbar::getRightOpening)
        ;

    //FieldLines: holds corner and line information
    class_<FieldLines, boost::shared_ptr<FieldLines> >("FieldLines", no_init)
        .def_readonly("numCorners", &FieldLines::getNumCorners)
        .def_readonly("numLines", &FieldLines::getNumLines)
        .add_property("lines", &FieldLines::getActualLines)
        .add_property("corners", &FieldLines::getActualCorners)
        ;

    //FieldLines helper classes:/

    // FieldLines holds a list of shared_ptrs to VisualLines (linesList)
    class_<std::vector<boost::shared_ptr<VisualLine> > >("LineVec")
        // True is for NoProxy, since boost::shared_ptrs don't need one
        .def(vector_indexing_suite<std::vector<boost::shared_ptr<VisualLine> >, true>())
        ;

    class_<VisualLine, boost::shared_ptr<VisualLine> >("VisualLine", no_init)
        .def_readonly("angle", &VisualLine::getAngle)
        .def_readonly("avgWidth", &VisualLine::getAvgWidth)
        .def_readonly("bearing", &VisualLine::getBearingDeg)
        .def_readonly("dist", &VisualLine::getDistance)
        .def_readonly("length", &VisualLine::getLength)
        .def_readonly("slope", &VisualLine::getSlope)
        .def_readonly("yInt", &VisualLine::getYIntercept)
        .def_readonly("possibilities", &VisualLine::getIDs)
        ;

    //VisualLine can return a vector of IDs from ConcreteLine
    class_<std::vector<lineID> >("LineIDVec")
        .def(vector_indexing_suite<std::vector<lineID> >())
        ;

    // From ConcreteLine.h, gives the ID of a line
    enum_<lineID>("lineID")
        // Ambiguous lines
        .value("UNKNOWN_LINE", UNKNOWN_LINE) //50
        .value("SIDE_OR_ENDLINE", SIDE_OR_ENDLINE)
        .value("SIDELINE_LINE", SIDELINE_LINE)
        .value("ENDLINE_LINE", ENDLINE_LINE)
        .value("GOALBOX_LINE", GOALBOX_LINE)
        .value("GOALBOX_SIDE_LINE", GOALBOX_SIDE_LINE) //55
        .value("GOALBOX_TOP_LINE", GOALBOX_TOP_LINE)
        // Distinct lines
        // Named by looking from center field out, left end is at blue goal
        .value("BLUE_GOAL_ENDLINE", BLUE_GOAL_ENDLINE)
        .value("YELLOW_GOAL_ENDLINE", YELLOW_GOAL_ENDLINE)
        .value("TOP_SIDELINE", TOP_SIDELINE)
        .value("BOTTOM_SIDELINE", BOTTOM_SIDELINE) //60
        .value("MIDLINE", MIDLINE)
        //Goalbox lines
        // Named as if you were the goalie, top of box = TOP_LINE
        .value("BLUE_GOALBOX_TOP_LINE", BLUE_GOALBOX_TOP_LINE)
        .value("BLUE_GOALBOX_LEFT_LINE", BLUE_GOALBOX_LEFT_LINE)
        .value("BLUE_GOALBOX_RIGHT_LINE", BLUE_GOALBOX_RIGHT_LINE)
        .value("YELLOW_GOALBOX_TOP_LINE", YELLOW_GOALBOX_TOP_LINE) //65
        .value("YELLOW_GOALBOX_LEFT_LINE", YELLOW_GOALBOX_LEFT_LINE)
        .value("YELLOW_GOALBOX_RIGHT_LINE", YELLOW_GOALBOX_RIGHT_LINE)
        ;

    // FieldLines holds a list of VisualCorners (not pointers) (cornersList)
    class_<std::list<VisualCorner> >("CornerList")
        .def("__iter__", boost::python::iterator<std::list<VisualCorner> >())
        ;

    class_<VisualCorner>("VisualCorner", no_init)
        // From VisualDetection
        .def_readonly("dist", &VisualCorner::getDistance)
        .def_readonly("bearing", &VisualCorner::getBearingDeg)
        .def_readonly("x", &VisualCorner::getX)
        .def_readonly("y", &VisualCorner::getY)
        .def_readonly("possibilities", &VisualCorner::getIDs)
        .def_readonly("angleX", &VisualCorner::getAngleXDeg)
        .def_readonly("angleY", &VisualCorner::getAngleYDeg)
        ;

    // VisualCorner can return a vector of IDs from ConcreteCorner
    class_<std::vector<cornerID> >("CornerIDVec")
        .def(vector_indexing_suite<std::vector<cornerID> >())
        ;

    // From ConcreteCorner.h, gives the ID of a corner
    enum_<cornerID>("cornerID")
        .value("L_INNER_CORNER", L_INNER_CORNER)
        .value("L_OUTER_CORNER", L_OUTER_CORNER)
        .value("T_CORNER", T_CORNER)
        .value("CENTER_CIRCLE", CENTER_CIRCLE)

        //FUZZY/CLEAR IDS start at 4
        .value("BLUE_GOAL_T", BLUE_GOAL_T)
        .value("YELLOW_GOAL_T", YELLOW_GOAL_T) //5
        .value("BLUE_GOAL_RIGHT_L_OR_YELLOW_GOAL_LEFT_L",
               BLUE_GOAL_RIGHT_L_OR_YELLOW_GOAL_LEFT_L)
        .value("BLUE_GOAL_LEFT_L_OR_YELLOW_GOAL_RIGHT_L",
               BLUE_GOAL_LEFT_L_OR_YELLOW_GOAL_RIGHT_L)
        .value("BLUE_CORNER_TOP_L_OR_YELLOW_CORNER_BOTTOM_L",
               BLUE_CORNER_TOP_L_OR_YELLOW_CORNER_BOTTOM_L)
        .value("BLUE_CORNER_BOTTOM_L_OR_YELLOW_CORNER_TOP_L",
               BLUE_CORNER_BOTTOM_L_OR_YELLOW_CORNER_TOP_L)
        .value("CORNER_INNER_L", CORNER_INNER_L) //10
        .value("GOAL_BOX_INNER_L", GOAL_BOX_INNER_L)
        .value("BLUE_GOAL_OUTER_L", BLUE_GOAL_OUTER_L)
        .value("YELLOW_GOAL_OUTER_L", YELLOW_GOAL_OUTER_L)
        .value("CENTER_T", CENTER_T)

        //SPECIFIC CORNER IDS start at 15
        .value("BLUE_CORNER_TOP_L", BLUE_CORNER_TOP_L) //15
        .value("BLUE_CORNER_BOTTOM_L", BLUE_CORNER_BOTTOM_L)
        .value("BLUE_GOAL_LEFT_T", BLUE_GOAL_LEFT_T)
        .value("BLUE_GOAL_RIGHT_T", BLUE_GOAL_RIGHT_T)
        .value("BLUE_GOAL_LEFT_L", BLUE_GOAL_LEFT_L)
        .value("BLUE_GOAL_RIGHT_L", BLUE_GOAL_RIGHT_L) //20
        .value("CENTER_BOTTOM_T", CENTER_BOTTOM_T)
        .value("CENTER_TOP_T", CENTER_TOP_T)
        .value("YELLOW_CORNER_BOTTOM_L", YELLOW_CORNER_BOTTOM_L)
        .value("YELLOW_CORNER_TOP_L", YELLOW_CORNER_TOP_L)
        .value("YELLOW_GOAL_LEFT_T", YELLOW_GOAL_LEFT_T) //25
        .value("YELLOW_GOAL_RIGHT_T", YELLOW_GOAL_RIGHT_T)
        .value("YELLOW_GOAL_LEFT_L", YELLOW_GOAL_LEFT_L)
        .value("YELLOW_GOAL_RIGHT_L", YELLOW_GOAL_RIGHT_L)
        .value("CORNER_NO_IDEA_ID", CORNER_NO_IDEA_ID)
        .value("TOP_CC", TOP_CC) // 30
        .value("BOTTOM_CC", BOTTOM_CC)
        ;

    ///////MAIN VISION CLASS/////////
    //noncopyable is required because vision has no public copy constructor
    class_<Vision, boost::shared_ptr<Vision>, boost::noncopyable >("Vision", no_init)
        //make_getter provides a getter for objects not pointers
        .add_property("ball", make_getter(&Vision::ball, return_value_policy
                                          <reference_existing_object>()))
        .add_property("yglp", make_getter(&Vision::yglp, return_value_policy
                                          <reference_existing_object>()))
        .add_property("ygrp", make_getter(&Vision::ygrp, return_value_policy
                                          <reference_existing_object>()))
        .add_property("bglp", make_getter(&Vision::bglp, return_value_policy
                                          <reference_existing_object>()))
        .add_property("bgrp", make_getter(&Vision::bgrp, return_value_policy
                                          <reference_existing_object>()))
        .add_property("fieldLines", make_getter(&Vision::fieldLines,
                                                return_value_policy
                                                <return_by_value>()))
        .add_property("fieldEdge", make_getter(&Vision::fieldEdge,
                                               return_value_policy
                                               <reference_existing_object>()))
        .add_property("red1", make_getter(&Vision::red1, return_value_policy
                                          <reference_existing_object>()))
        .add_property("red2", make_getter(&Vision::red2, return_value_policy
                                          <reference_existing_object>()))
        .add_property("red3", make_getter(&Vision::red3, return_value_policy
                                          <reference_existing_object>()))
        .add_property("navy1", make_getter(&Vision::navy1, return_value_policy
                                          <reference_existing_object>()))
        .add_property("navy2", make_getter(&Vision::navy2, return_value_policy
                                          <reference_existing_object>()))
        .add_property("navy3", make_getter(&Vision::navy3, return_value_policy
                                          <reference_existing_object>()))
        .add_property("cross", make_getter(&Vision::cross, return_value_policy
                                           <reference_existing_object>()))

        /* Crossbars: not used right now
           .add_property("ygCrossbar", make_getter(&Vision::ygCrossbar,
           return_value_policy<reference_existing_object>()))
           .add_property("bgCrossbar", make_getter(&Vision::bgCrossbar,
           return_value_policy<reference_existing_object>()))
        */
        ;

    scope().attr("vision") = vision_pointer;
}

void c_init_vision() {
    if (!Py_IsInitialized())
        Py_Initialize();
    try{
        initvision();
    } catch (error_already_set) {
        PyErr_Print();
    }
}

void set_vision_pointer (boost::shared_ptr<Vision> visionptr) {
    vision_pointer = visionptr;
}






